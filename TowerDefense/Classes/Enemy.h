#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "Waypoint.h"
#include "Tower.h"

class Enemy : public cocos2d::CCNode
{
public:
	Enemy(void);
	~Enemy(void);

	static Enemy* nodeWithTheGame(HelloWorld* game);
	bool initWithTheGame(HelloWorld* game);
	void doActivate(float dt);
	void getRemoved();
	void getAttacked(Tower* attacker);
	void gotLostSight(Tower* attacker);
	void getDamaged(int damage);

	void update(float dt);
	void draw(void);

	CC_SYNTHESIZE(HelloWorld*, _theGame, TheGame);
	CC_SYNTHESIZE(cocos2d::CCSprite*, _mySprite, MySprite);

private:
	cocos2d::CCPoint myPosition;
	int maxHp;
	int currentHp;
	float walkingSpeed;
	Waypoint *destinationWaypoint;
	bool active;
	cocos2d::CCArray *attackedBy;
};

#endif  // __ENEMY_H__