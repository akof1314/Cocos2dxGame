#include "Tower.h"
#include "Enemy.h"
using namespace cocos2d;

Tower::Tower(void)
{
}

Tower::~Tower(void)
{
}

Tower* Tower::nodeWithTheGame(HelloWorld* game, CCPoint location)
{
	Tower *pRet = new Tower();
	if (pRet && pRet->initWithTheGame(game, location))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool Tower::initWithTheGame(HelloWorld* game, CCPoint location)
{
	bool bRet = false;
	do 
	{
		attackRange = 70;
		damage = 10;
		fireRate = 1;
		chosenEnemy = NULL;
		
		_mySprite = CCSprite::create("tower.png");
		this->addChild(_mySprite);
		_mySprite->setPosition(location);
		_theGame = game;
		_theGame->addChild(this);

		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void Tower::update(float dt)
{
	if (chosenEnemy)
	{
		//We make it turn to target the enemy chosen
		CCPoint normalized = ccpNormalize(ccp(chosenEnemy->getMySprite()->getPosition().x - _mySprite->getPosition().x,
			chosenEnemy->getMySprite()->getPosition().y - _mySprite->getPosition().y));
		_mySprite->setRotation(CC_RADIANS_TO_DEGREES(atan2(normalized.y, - normalized.x)) + 90);

		if (!_theGame->collisionWithCircle(_mySprite->getPosition(), attackRange, chosenEnemy->getMySprite()->getPosition(), 1))
		{
			this->lostSightOfEnemy();
		}
	} 
	else
	{
		CCObject *pObject = NULL;
		CCARRAY_FOREACH(_theGame->getEnemies(), pObject)
		{
			Enemy *enemy = (Enemy*)pObject;
			if (_theGame->collisionWithCircle(_mySprite->getPosition(), attackRange, enemy->getMySprite()->getPosition(), 1))
			{
				this->chosenEnemyForAttack(enemy);
				break;
			}			
		}
	}
}

void Tower::draw(void)
{
#ifdef COCOS2D_DEBUG
	ccDrawColor4F(255, 255, 255, 255);
	ccDrawCircle(_mySprite->getPosition(), attackRange, 360, 30, false);
#endif
	CCNode::draw();
}

void Tower::attackEnemy()
{
	this->schedule(schedule_selector(Tower::shootWeapon), fireRate);
}

void Tower::chosenEnemyForAttack(Enemy *enemy)
{
	chosenEnemy = NULL;
	chosenEnemy = enemy;
	this->attackEnemy();
	enemy->getAttacked(this);
}

void Tower::shootWeapon(float dt)
{
	CCSprite *bullet = CCSprite::create("bullet.png");
	_theGame->addChild(bullet);
	bullet->setPosition(_mySprite->getPosition());
	bullet->runAction(CCSequence::create(
		CCMoveTo::create(0.1, chosenEnemy->getMySprite()->getPosition()),
		CCCallFunc::create(this, callfunc_selector(Tower::damageEnemy)),
		CCCallFuncN::create(this, callfuncN_selector(Tower::removeBullet)),
		NULL));
}

void Tower::removeBullet(CCSprite *bullet)
{
	bullet->getParent()->removeChild(bullet, true);
}

void Tower::damageEnemy()
{
	if (chosenEnemy)
	{
		chosenEnemy->getDamaged(damage);
	}
}

void Tower::targetKilled()
{
	if (chosenEnemy)
	{
		chosenEnemy = NULL;
	}

	this->unschedule(schedule_selector(Tower::shootWeapon));
}

void Tower::lostSightOfEnemy()
{
	chosenEnemy->gotLostSight(this);
	if (chosenEnemy)
	{
		chosenEnemy = NULL;
	}

	this->unschedule(schedule_selector(Tower::shootWeapon));
}
