#include "CatSprite.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

CatSprite::CatSprite() :
    _numBones(0),
    _facingForwardAnimation(nullptr),
    _facingBackAnimation(nullptr),
    _facingLeftAnimation(nullptr),
    _facingRightAnimation(nullptr),
    _curAnimation(nullptr),
    _curAnimate(nullptr)
{
}

CatSprite::~CatSprite()
{
    CC_SAFE_RELEASE_NULL(_facingForwardAnimation);
    CC_SAFE_RELEASE_NULL(_facingBackAnimation);
    CC_SAFE_RELEASE_NULL(_facingLeftAnimation);
    CC_SAFE_RELEASE_NULL(_facingRightAnimation);
}

CatSprite *CatSprite::createWithLayer(HelloWorld *layer)
{
    CatSprite *pRet = new CatSprite();
    if (pRet && pRet->initWithLayer(layer))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool CatSprite::initWithLayer(HelloWorld *layer)
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(!this->initWithSpriteFrameName("cat_forward_1.png"));
        
        _layer = layer;
        
        _facingForwardAnimation = this->createCatAnimation("forward");
        _facingForwardAnimation->retain();
        _facingBackAnimation = this->createCatAnimation("back");
        _facingBackAnimation->retain();
        _facingLeftAnimation = this->createCatAnimation("left");
        _facingLeftAnimation->retain();
        _facingRightAnimation = this->createCatAnimation("right");
        _facingRightAnimation->retain();

        bRet = true;
    } while (0);

    return bRet;
}

void CatSprite::moveToward(const Point &target)
{
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    Point toTileCoord = _layer->tileCoordForPosition(target);

    if (fromTileCoord == toTileCoord)
    {
        CCLOG("You're already there! :P");
        return;
    }

    if (!_layer->isValidTileCoord(toTileCoord) || _layer->isWallAtTileCoord(toTileCoord))
    {
        SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
        return;
    }

    CCLOG("From: %f, %f", fromTileCoord.x, fromTileCoord.y);
    CCLOG("To: %f, %f", toTileCoord.x, toTileCoord.y);

    //bool pathFound = false;
    _spOpenSteps.clear();
    _spClosedSteps.clear();

    // 首先，添加猫的方块坐标到open列表
    this->insertInOpenSteps(ShortestPathStep::createWithPosition(fromTileCoord));

    do
    {
        // 得到最小的F值步骤
        // 因为是有序列表，第一个步骤总是最小的F值
        ShortestPathStep *currentStep = _spOpenSteps.at(0);

        // 添加当前步骤到closed列表
        _spClosedSteps.pushBack(currentStep);

        // 将它从open列表里面移除
        // 需要注意的是，如果想要先从open列表里面移除，应小心对象的内存
        _spOpenSteps.erase(0);

        // 如果当前步骤是目标方块坐标，那么就完成了
        if (currentStep->getPosition() == toTileCoord)
        {
            //pathFound = true;
            this->constructPathAndStartAnimationFromStep(currentStep);
            //ShortestPathStep *tmpStep = currentStep;
            //CCLOG("PATH FOUND :");
            //do
            //{
            //    CCLOG("%s", tmpStep->getDescription().c_str());
            //    tmpStep = tmpStep->getParent(); // 倒退
            //} while (tmpStep);                  // 直到没有上一步

            _spOpenSteps.clear();
            _spClosedSteps.clear();
            break;
        }

        // 得到当前步骤的相邻方块坐标
        PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(currentStep->getPosition());
        for (ssize_t i = 0; i < adjSteps->count(); ++i)
        {
            ShortestPathStep *step = ShortestPathStep::createWithPosition(adjSteps->getControlPointAtIndex(i));

            // 检查步骤是不是已经在closed列表
            if (_spClosedSteps.contains(step))
            {
                continue;
            }

            // 计算从当前步骤到此步骤的成本
            int moveCost = this->costToMoveFromStepToAdjacentStep(currentStep, step);

            // 检查此步骤是否已经在open列表
            ssize_t index = _spOpenSteps.getIndex(step);

            // 不在open列表，添加它
            if (index == -1)
            {
                // 设置当前步骤作为上一步操作
                step->setParent(currentStep);

                // G值等同于上一步的G值 + 从上一步到这里的成本
                step->setGScore(currentStep->getGScore() + moveCost);

                // H值即是从此步骤到目标方块坐标的移动量估算值
                step->setHScore(this->computeHScoreFromCoordToCoord(step->getPosition(), toTileCoord));

                // 按序添加到open列表
                this->insertInOpenSteps(step);
            }
            else
            {
                // 获取旧的步骤，其值已经计算过
                step = _spOpenSteps.at(index);

                // 检查G值是否低于当前步骤到此步骤的值
                if ((currentStep->getGScore() + moveCost) < step->getGScore())
                {
                    // G值等同于上一步的G值 + 从上一步到这里的成本
                    step->setGScore(currentStep->getGScore() + moveCost);

                    // 因为G值改变了，F值也会跟着改变
                    // 所以为了保持open列表有序，需要将此步骤移除，再重新按序插入

                    // 在移除之前，需要先保持引用
                    step->retain();

                    // 现在可以放心移除，不用担心被释放
                    _spOpenSteps.erase(index);

                    // 重新按序插入
                    this->insertInOpenSteps(step);

                    // 现在可以释放它了，因为open列表应该持有它
                    step->release();
                }
            }
        }
    } while (_spOpenSteps.size() > 0);

    //if (!pathFound)
    if (_shortestPath.empty())
    {
        SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
    }
}

Animation *CatSprite::createCatAnimation(const char *animType)
{
    Animation *animation = Animation::create();
    for (int i = 1; i <= 2; ++i)
    {
        animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("cat_%s_%d.png", animType, i)));
    }
    animation->setDelayPerUnit(0.2f);
    return animation;
}

void CatSprite::runAnimation(Animation *animation)
{
    if (_curAnimation == animation)
    {
        return;
    }
    
    _curAnimation = animation;
    if (_curAnimate != nullptr)
    {
        this->stopAction(_curAnimate);
    }
    
    _curAnimate = RepeatForever::create(Animate::create(animation));
    this->runAction(_curAnimate);
}

void CatSprite::insertInOpenSteps(CatSprite::ShortestPathStep *step)
{
    int stepFScore = step->getFScore();
    ssize_t count = _spOpenSteps.size();
    ssize_t i = 0;
    for (; i < count; ++i)
    {
        if (stepFScore <= _spOpenSteps.at(i)->getFScore())
        {
            break;
        }
    }
    _spOpenSteps.insert(i, step);
}

int CatSprite::computeHScoreFromCoordToCoord(const Point &fromCoord, const Point &toCoord)
{
    // 这里使用曼哈顿方法，计算从当前步骤到达目标步骤，在水平和垂直方向总的步数
    // 忽略了可能在路上的各种障碍
    return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}

int CatSprite::costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep)
{
    return ((fromStep->getPosition().x != toStep->getPosition().x)
            && (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}

void CatSprite::constructPathAndStartAnimationFromStep(CatSprite::ShortestPathStep *step)
{
    _shortestPath.clear();

    do
    {
        // 起始位置不要进行添加
        if (step->getParent())
        {
            // 总是插入到索引0的位置，以便反转路径
            _shortestPath.insert(0, step);
        }
        step = step->getParent();   // 倒退
    } while (step);                 // 直到没有上一步

    for (const ShortestPathStep *s : _shortestPath)
    {
        CCLOG("%s", s->getDescription().c_str());
    }

    this->popStepAndAnimate();
}

void CatSprite::popStepAndAnimate()
{
    Point currentPosition = _layer->tileCoordForPosition(this->getPosition());

    if (_layer->isBoneAtTilecoord(currentPosition))
    {
        SimpleAudioEngine::getInstance()->playEffect("pickup.wav");
        _numBones++;
        _layer->showNumBones(_numBones);
        _layer->removeObjectAtTileCoord(currentPosition);
    }
    else if (_layer->isDogAtTilecoord(currentPosition))
    {
        if (_numBones == 0)
        {
            _layer->loseGame();
            return;
        }
        else
        {
            _numBones--;
            _layer->showNumBones(_numBones);
            _layer->removeObjectAtTileCoord(currentPosition);
            SimpleAudioEngine::getInstance()->playEffect("catAttack.wav");
        }
    }
    else if (_layer->isExitAtTilecoord(currentPosition))
    {
        _layer->winGame();
        return;
    }
    else
    {
        SimpleAudioEngine::getInstance()->playEffect("step.wav");
    }

    // 检查是否仍有路径步骤需要前进
    if (_shortestPath.size() == 0)
    {
        return;
    }

    // 得到下一步移动的步骤
    ShortestPathStep *s = _shortestPath.at(0);

    Point futurePosition = s->getPosition();
    Point diff = futurePosition - currentPosition;
    if (abs(diff.x) > abs(diff.y))
    {
        if (diff.x > 0)
        {
            this->runAnimation(_facingRightAnimation);
        }
        else
        {
            this->runAnimation(_facingLeftAnimation);
        }
    }
    else
    {
        if (diff.y > 0)
        {
            this->runAnimation(_facingForwardAnimation);
        }
        else
        {
            this->runAnimation(_facingBackAnimation);
        }
    }

    // 准备动作和回调
    MoveTo *moveAction = MoveTo::create(0.4f, _layer->positionForTileCoord(s->getPosition()));
    CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(CatSprite::popStepAndAnimate, this));

    // 移除步骤
    _shortestPath.erase(0);

    // 运行动作
    this->runAction(Sequence::create(moveAction, moveCallback, nullptr));
}

CatSprite::ShortestPathStep::ShortestPathStep() :
    _position(Point::ZERO),
    _gScore(0),
    _hScore(0),
    _parent(nullptr)
{
}

CatSprite::ShortestPathStep::~ShortestPathStep()
{
}

CatSprite::ShortestPathStep *CatSprite::ShortestPathStep::createWithPosition(const Point &pos)
{
    ShortestPathStep *pRet = new ShortestPathStep();
    if (pRet && pRet->initWithPosition(pos))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool CatSprite::ShortestPathStep::initWithPosition(const Point &pos)
{
    bool bRet = false;
    do
    {
        this->setPosition(pos);

        bRet = true;
    } while (0);

    return bRet;
}

int CatSprite::ShortestPathStep::getFScore() const
{
    return this->getGScore() + this->getHScore();
}

bool CatSprite::ShortestPathStep::isEqual(const CatSprite::ShortestPathStep *other) const
{
    return this->getPosition() == other->getPosition();
}

std::string CatSprite::ShortestPathStep::getDescription() const
{
    return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
                               this->getPosition().x, this->getPosition().y,
                               this->getGScore(), this->getHScore(), this->getFScore());
}
