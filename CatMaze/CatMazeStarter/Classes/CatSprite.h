#ifndef __CAT_SPRITE_H__
#define __CAT_SPRITE_H__

#include "cocos2d.h"

class HelloWorld;

class CatSprite : public cocos2d::Sprite
{
public:
    CatSprite();
    ~CatSprite();
    
    static CatSprite *createWithLayer(HelloWorld *layer);
    bool initWithLayer(HelloWorld *layer);

    void moveToward(const cocos2d::Point &target);
    
    CC_SYNTHESIZE_READONLY(int, _numBones, NumBones);
    
private:
    cocos2d::Animation *createCatAnimation(const char *animType);
    void runAnimation(cocos2d::Animation *animation);
    
private:
    HelloWorld *_layer;
    cocos2d::Animation *_facingForwardAnimation;
    cocos2d::Animation *_facingBackAnimation;
    cocos2d::Animation *_facingLeftAnimation;
    cocos2d::Animation *_facingRightAnimation;
    cocos2d::Animation *_curAnimation;
    cocos2d::Action *_curAnimate;
};

#endif // __CAT_SPRITE_H__
