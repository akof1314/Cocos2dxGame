#include "Enemy.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;

#define HEALTH_BAR_WIDTH 20
#define HEALTH_BAR_ORIGIN -10

Enemy::Enemy(void)
{
}

Enemy::~Enemy(void)
{
}

Enemy* Enemy::nodeWithTheGame(HelloWorld* game)
{
	Enemy *pRet = new Enemy();
	if (pRet && pRet->initWithTheGame(game))
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

bool Enemy::initWithTheGame(HelloWorld* game)
{
	bool bRet = false;
	do 
	{
		attackedBy = CCArray::createWithCapacity(5);
		attackedBy->retain();
		maxHp = 40;
		currentHp = maxHp;
		active = false;
		walkingSpeed = 0.5;

		_theGame = game;
		_mySprite = CCSprite::create("enemy.png");
		this->addChild(_mySprite);

		Waypoint *waypoint = (Waypoint*)_theGame->getWaypoints()->objectAtIndex(_theGame->getWaypoints()->count() - 1);
		destinationWaypoint = waypoint->getNextWaypoint();
		CCPoint pos = waypoint->getMyPosition();
		myPosition = pos;
		_mySprite->setPosition(pos);
		_theGame->addChild(this);

		this->scheduleUpdate();

		bRet = true;
	} while (0);

	return bRet;
}

void Enemy::doActivate(float dt)
{
	active = true;
}

void Enemy::getRemoved()
{
	CCObject *pObject = NULL;
	CCARRAY_FOREACH(attackedBy, pObject)
	{
		Tower *attacker = (Tower*)pObject;
		attacker->targetKilled();
	}

	this->getParent()->removeChild(this, true);
	_theGame->getEnemies()->removeObject(this);

	//Notify the game that we killed an enemy so we can check if we can send another wave
	_theGame->enemyGotKilled();
}

void Enemy::update(float dt)
{
	if (!active)
	{
		return;
	}

	if (_theGame->collisionWithCircle(myPosition, 1, destinationWaypoint->getMyPosition(), 1))
	{
		if (destinationWaypoint->getNextWaypoint())
		{
			destinationWaypoint = destinationWaypoint->getNextWaypoint();
		} 
		else
		{
			//Reached the end of the road. Damage the player
			_theGame->getHpDamage();
			this->getRemoved();
		}
	}

	CCPoint targetPoint =  destinationWaypoint->getMyPosition();
	float movementSpeed = walkingSpeed;

	CCPoint normalized = ccpNormalize(ccp(targetPoint.x - myPosition.x, targetPoint.y - myPosition.y));
	_mySprite->setRotation(CC_RADIANS_TO_DEGREES(atan2(normalized.y, - normalized.x)));

	myPosition = ccp(myPosition.x + normalized.x * movementSpeed, myPosition.y + normalized.y * movementSpeed);
	_mySprite->setPosition(myPosition);
}

void Enemy::draw(void)
{
	CCPoint healthBarBack[] = {
		ccp(_mySprite->getPosition().x - 10, _mySprite->getPosition().y + 16),
		ccp(_mySprite->getPosition().x + 10, _mySprite->getPosition().y + 16),
		ccp(_mySprite->getPosition().x + 10, _mySprite->getPosition().y + 14),
		ccp(_mySprite->getPosition().x - 10, _mySprite->getPosition().y + 14)
	};
	ccDrawSolidPoly(healthBarBack, 4, ccc4f(255, 0, 0, 255));

	CCPoint healthBar[] = {
		ccp(_mySprite->getPosition().x + HEALTH_BAR_ORIGIN, _mySprite->getPosition().y + 16),
		ccp(_mySprite->getPosition().x + HEALTH_BAR_ORIGIN + (float)(currentHp * HEALTH_BAR_WIDTH) / maxHp, _mySprite->getPosition().y + 16),
		ccp(_mySprite->getPosition().x + HEALTH_BAR_ORIGIN + (float)(currentHp * HEALTH_BAR_WIDTH) / maxHp, _mySprite->getPosition().y + 14),
		ccp(_mySprite->getPosition().x + HEALTH_BAR_ORIGIN, _mySprite->getPosition().y + 14)
	};
	ccDrawSolidPoly(healthBar, 4, ccc4f(0, 255, 0, 255));

	CCNode::draw();
}

void Enemy::getAttacked(Tower* attacker)
{
	attackedBy->addObject(attacker);
}

void Enemy::gotLostSight(Tower* attacker)
{
	attackedBy->removeObject(attacker);
}

void Enemy::getDamaged(int damage)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("laser_shoot.wav");

	currentHp -= damage;
	if (currentHp <= 0)
	{
		_theGame->awardGold(200);
		this->getRemoved();
	}
}
