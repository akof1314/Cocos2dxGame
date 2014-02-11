#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class CatSprite;

class HelloWorld : public cocos2d::Layer
{
public:
    HelloWorld();
    ~HelloWorld();
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();      
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    bool isWallAtTileCoord(const cocos2d::Point &tileCoord) const;
    bool isBoneAtTilecoord(const cocos2d::Point &tileCoord) const;
    bool isDogAtTilecoord(const cocos2d::Point &tileCoord) const;
    bool isExitAtTilecoord(const cocos2d::Point &tileCoord) const;
    cocos2d::Point tileCoordForPosition(const cocos2d::Point &position) const;
    cocos2d::Point positionForTileCoord(const cocos2d::Point &tileCoord) const;
    void removeObjectAtTileCoord(const cocos2d::Point &tileCoord);
    void winGame();
    void loseGame();
    void showNumBones(int numBones);
    bool isValidTileCoord(const cocos2d::Point &tileCoord) const;
    cocos2d::PointArray *walkableAdjacentTilesCoordForTileCoord(const cocos2d::Point &tileCoord) const;
    
private:
    bool isPropAtTileCoordForLayer(const char *prop, const cocos2d::Point &tileCoord, cocos2d::TMXLayer *layer) const;
    void setViewpointCenter(const cocos2d::Point &position);
    void showRestartMenu();
    void endScene();

    virtual void update(float delta) override;
    
private:
    cocos2d::TMXTiledMap *_tileMap;
    cocos2d::TMXLayer *_bgLayer;
    cocos2d::TMXLayer *_objectLayer;
    cocos2d::SpriteBatchNode *_batchNode;
    CatSprite *_cat;
    bool _gameOver;
    bool _won;
    cocos2d::LabelBMFont *_bonesCount;
};

#endif // __HELLOWORLD_SCENE_H__
