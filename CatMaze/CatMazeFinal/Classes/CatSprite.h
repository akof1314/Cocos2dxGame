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

private:
    class ShortestPathStep : public cocos2d::Object
    {
    public:
        ShortestPathStep();
        ~ShortestPathStep();

        static ShortestPathStep *createWithPosition(const cocos2d::Point &pos);
        bool initWithPosition(const cocos2d::Point &pos);

        int getFScore() const;
        bool isEqual(const ShortestPathStep *other) const;
        std::string getDescription() const;

        CC_SYNTHESIZE(cocos2d::Point, _position, Position);
        CC_SYNTHESIZE(int, _gScore, GScore);
        CC_SYNTHESIZE(int, _hScore, HScore);
        CC_SYNTHESIZE(ShortestPathStep*, _parent, Parent);
    };

    cocos2d::Vector<ShortestPathStep*> _spOpenSteps;
    cocos2d::Vector<ShortestPathStep*> _spClosedSteps;
    cocos2d::Vector<ShortestPathStep*> _shortestPath;

    void insertInOpenSteps(ShortestPathStep *step);
    int computeHScoreFromCoordToCoord(const cocos2d::Point &fromCoord, const cocos2d::Point &toCoord);
    int costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep);
    ssize_t getStepIndex(const cocos2d::Vector<ShortestPathStep*> &steps, const ShortestPathStep *step);
    void constructPathAndStartAnimationFromStep(ShortestPathStep *step);
    void popStepAndAnimate();
};

#endif // __CAT_SPRITE_H__
