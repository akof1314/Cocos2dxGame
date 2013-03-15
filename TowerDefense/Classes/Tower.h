#ifndef __TOWER_H__
#define __TOWER_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"

#define kTOWER_COST 300

class Enemy;

class Tower : public cocos2d::CCNode
{
public:
	Tower(void);
	~Tower(void);

	static Tower* nodeWithTheGame(HelloWorld* game, cocos2d::CCPoint location);
	bool initWithTheGame(HelloWorld* game, cocos2d::CCPoint location);
	void attackEnemy();
	void chosenEnemyForAttack(Enemy *enemy);
	void shootWeapon(float dt);
	void removeBullet(cocos2d::CCSprite *bullet);
	void damageEnemy();
	void targetKilled();
	void lostSightOfEnemy();

	void update(float dt);
	void draw(void);

	CC_SYNTHESIZE(HelloWorld*, _theGame, TheGame);
	CC_SYNTHESIZE(cocos2d::CCSprite*, _mySprite, MySprite);

private:
	int attackRange;
	int damage;
	float fireRate;
	bool attacking;
	Enemy *chosenEnemy;
};

#endif  // __TOWER_H__