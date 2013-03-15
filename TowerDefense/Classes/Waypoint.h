#ifndef __WAYPOINT_H__
#define __WAYPOINT_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"

class Waypoint : public cocos2d::CCNode
{
public:
	Waypoint(void);
	~Waypoint(void);

	static Waypoint* nodeWithTheGame(HelloWorld* game, cocos2d::CCPoint location);
	bool initWithTheGame(HelloWorld* game, cocos2d::CCPoint location);

	void draw(void);

	CC_SYNTHESIZE(cocos2d::CCPoint, _myPosition, MyPosition);
	CC_SYNTHESIZE(Waypoint*, _nextWaypoint, NextWaypoint);

private:
	HelloWorld* theGame;
};

#endif  // __WAYPOINT_H__