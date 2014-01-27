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
    Point diff = target - this->getPosition();
    Point desiredTileCoord = _layer->tileCoordForPosition(this->getPosition());

    if (abs(diff.x) > abs(diff.y))
    {
        if (diff.x > 0)
        {
            desiredTileCoord.x += 1;
            this->runAnimation(_facingRightAnimation);
        }
        else
        {
            desiredTileCoord.x -= 1;
            this->runAnimation(_facingLeftAnimation);
        }
    }
    else
    {
        if (diff.y > 0)
        {
            desiredTileCoord.y -= 1;
            this->runAnimation(_facingBackAnimation);
        }
        else
        {
            desiredTileCoord.y += 1;
            this->runAnimation(_facingForwardAnimation);
        }
    }

    if (!_layer->isValidTileCoord(desiredTileCoord) || _layer->isWallAtTileCoord(desiredTileCoord))
    {
        SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
    }
    else
    {
        this->setPosition(_layer->positionForTileCoord(desiredTileCoord));

        if (_layer->isBoneAtTilecoord(desiredTileCoord))
        {
            SimpleAudioEngine::getInstance()->playEffect("pickup.wav");
            _numBones++;
            _layer->showNumBones(_numBones);
            _layer->removeObjectAtTileCoord(desiredTileCoord);
        }
        else if (_layer->isDogAtTilecoord(desiredTileCoord))
        {
            if (_numBones == 0)
            {
                _layer->loseGame();
            }
            else
            {
                _numBones--;
                _layer->showNumBones(_numBones);
                _layer->removeObjectAtTileCoord(desiredTileCoord);
                SimpleAudioEngine::getInstance()->playEffect("catAttack.wav");
            }
        }
        else if (_layer->isExitAtTilecoord(desiredTileCoord))
        {
            _layer->winGame();
        }
        else
        {
            SimpleAudioEngine::getInstance()->playEffect("step.wav");
        }
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
