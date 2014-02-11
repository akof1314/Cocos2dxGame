#include "HelloWorldScene.h"
#include "CatSprite.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::HelloWorld():
    _tileMap(nullptr),
    _bgLayer(nullptr),
    _objectLayer(nullptr),
    _batchNode(nullptr),
    _cat(nullptr),
    _gameOver(false),
    _won(false),
    _bonesCount(nullptr)
{
}

HelloWorld::~HelloWorld()
{
}

bool HelloWorld::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(!CCLayer::init());

        _tileMap = TMXTiledMap::create("CatMaze.tmx");
        this->addChild(_tileMap);

        Point spawnTileCoord(24, 0);
        Point spawnPos = this->positionForTileCoord(spawnTileCoord);
        this->setViewpointCenter(spawnPos);

        SimpleAudioEngine::getInstance()->playBackgroundMusic("SuddenDefeat.mp3", true);
        SimpleAudioEngine::getInstance()->preloadEffect("pickup.wav");
        SimpleAudioEngine::getInstance()->preloadEffect("catAttack.wav");
        SimpleAudioEngine::getInstance()->preloadEffect("hitWall.wav");
        SimpleAudioEngine::getInstance()->preloadEffect("lose.wav");
        SimpleAudioEngine::getInstance()->preloadEffect("step.wav");
        SimpleAudioEngine::getInstance()->preloadEffect("win.wav");

        _bgLayer = _tileMap->getLayer("Background");
        _objectLayer = _tileMap->getLayer("Objects");

        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("CatMaze.plist");
        _batchNode = SpriteBatchNode::create("CatMaze.png");
        _tileMap->addChild(_batchNode);
        _cat = CatSprite::createWithLayer(this);
        _cat->setPosition(spawnPos);
        _batchNode->addChild(_cat);

        _bonesCount = LabelBMFont::create("Bones: 0", "Arial.fnt");
        _bonesCount->setPosition(400, 30);
        this->addChild(_bonesCount);

        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this](Touch *touch, Event *event){
            if (_gameOver)
            {
                return false;
            }

            Point touchLocation = _tileMap->convertTouchToNodeSpace(touch);
            _cat->moveToward(touchLocation);
            return true;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        this->scheduleUpdate();

        bRet = true;
    } while (0);
    return bRet;
}

bool HelloWorld::isWallAtTileCoord(const Point &tileCoord) const
{
    return this->isPropAtTileCoordForLayer("Wall", tileCoord, _bgLayer);
}

bool HelloWorld::isBoneAtTilecoord(const Point &tileCoord) const
{
    return this->isPropAtTileCoordForLayer("Bone", tileCoord, _objectLayer);
}

bool HelloWorld::isDogAtTilecoord(const Point &tileCoord) const
{
    return this->isPropAtTileCoordForLayer("Dog", tileCoord, _objectLayer);
}

bool HelloWorld::isExitAtTilecoord(const Point &tileCoord) const
{
    return this->isPropAtTileCoordForLayer("Exit", tileCoord, _objectLayer);
}

Point HelloWorld::tileCoordForPosition(const Point &position) const
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return Point(x, y);
}

Point HelloWorld::positionForTileCoord(const Point &tileCoord) const
{
    int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width / 2;
    int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) -
            (tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height / 2;
    return Point(x, y);
}

void HelloWorld::removeObjectAtTileCoord(const Point &tileCoord)
{
    _objectLayer->removeTileAt(tileCoord);
}

void HelloWorld::winGame()
{
    _gameOver = true;
    _won = true;
    SimpleAudioEngine::getInstance()->playEffect("win.wav");
    this->endScene();
}

void HelloWorld::loseGame()
{
    _gameOver = true;
    _won = false;
    SimpleAudioEngine::getInstance()->playEffect("lose.wav");
    this->endScene();
}

void HelloWorld::showNumBones(int numBones)
{
    _bonesCount->setString(StringUtils::format("Bones: %d", numBones));
}

bool HelloWorld::isPropAtTileCoordForLayer(const char *prop, const Point &tileCoord, TMXLayer *layer) const
{
    if (!this->isValidTileCoord(tileCoord))
    {
        return false;
    }
    
    int gid = layer->getTileGIDAt(tileCoord);
    Value properties = _tileMap->getPropertiesForGID(gid);
    if (properties.isNull())
    {
        return false;
    }
    return properties.asValueMap().find(prop) != properties.asValueMap().end();
}

bool HelloWorld::isValidTileCoord(const Point &tileCoord) const
{
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= _tileMap->getMapSize().width ||
        tileCoord.y >= _tileMap->getMapSize().height)
    {
        return false;
    }
    else
    {
        return true;
    }
}

PointArray *HelloWorld::walkableAdjacentTilesCoordForTileCoord(const Point &tileCoord) const
{
    PointArray *tmp = PointArray::create(8);

    bool t = false;
    bool l = false;
    bool b = false;
    bool r = false;

    // 上
    Point p(tileCoord.x, tileCoord.y - 1);
    if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
        t = true;
    }

    // 左
    p.setPoint(tileCoord.x - 1, tileCoord.y);
    if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
        l = true;
    }

    // 下
    p.setPoint(tileCoord.x, tileCoord.y + 1);
    if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
        b = true;
    }

    // 右
    p.setPoint(tileCoord.x + 1, tileCoord.y);
    if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
        r = true;
    }

    // 左上
    p.setPoint(tileCoord.x - 1, tileCoord.y - 1);
    if (t && l && this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
    }

    // 左下
    p.setPoint(tileCoord.x - 1, tileCoord.y + 1);
    if (b && l && this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
    }

    // 右上
    p.setPoint(tileCoord.x + 1, tileCoord.y - 1);
    if (t && r && this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
    }

    // 右下
    p.setPoint(tileCoord.x + 1, tileCoord.y + 1);
    if (b && r && this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
    {
        tmp->addControlPoint(p);
    }

    return tmp;
}

void HelloWorld::setViewpointCenter(const Point &position)
{
    Size winSize = Director::getInstance()->getWinSize();

    int x = MAX(position.x, winSize.width / 2);
    int y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (_tileMap->getMapSize().width * _tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
    Point actualPosition(x, y);

    Point centerOfView(winSize.width / 2, winSize.height / 2);
    Point viewPoint = centerOfView - actualPosition;

    _tileMap->setPosition(viewPoint);
}

void HelloWorld::showRestartMenu()
{
    Size winSize = Director::getInstance()->getWinSize();

    const char *message;
    if (_won)
    {
        message = "You win!";
    }
    else
    {
        message = "You lose!";
    }

    LabelBMFont *label = LabelBMFont::create(message, "Arial.fnt");
    label->setScale(0.1f);
    label->setPosition(winSize.width / 2, winSize.height * 0.6f);
    this->addChild(label);

    LabelBMFont *restartLabel = LabelBMFont::create("Restart", "Arial.fnt");
    MenuItemLabel *restartItem = MenuItemLabel::create(restartLabel, [](Object *sender){
        Director::getInstance()->replaceScene(TransitionZoomFlipX::create(0.5f, HelloWorld::createScene()));
    });
    restartItem->setScale(0.1f);
    restartItem->setPosition(winSize.width / 2, winSize.height * 0.4f);

    Menu *menu = Menu::createWithItem(restartItem);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);

    restartItem->runAction(ScaleTo::create(0.5f, 1.0f));
    label->runAction(ScaleTo::create(0.5f, 1.0f));
}

void HelloWorld::endScene()
{
    _cat->runAction(Sequence::create(ScaleBy::create(0.5f, 3.0f),
                                     DelayTime::create(1.0f),
                                     ScaleTo::create(0.5f, 0.0f),
                                     CallFunc::create(CC_CALLBACK_0(HelloWorld::showRestartMenu, this)),
                                     nullptr));
    _cat->runAction(RepeatForever::create(RotateBy::create(0.5f, 360.0f)));
}

void HelloWorld::update(float delta)
{
    this->setViewpointCenter(_cat->getPosition());
}

