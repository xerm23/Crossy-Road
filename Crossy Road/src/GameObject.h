#ifndef  GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Globals.h"


class GameObject {
private:
	int id = -1;
	bool enemy = false;
	float moveSpeed = 0;

	int bombDistance = 0;


public:
	int movingDirection = left;
	int modelID = 0;
	GameObject() {};
	~GameObject() {};
	GameObject(int ID) : id(ID) {};
	GameObject(int ID, float movespeed) : id(ID), moveSpeed(movespeed) {};
	GameObject(int ID, int model) : id(ID), modelID(model) {};

	array<GameObject*> rowObjects;
	array<GameObject*> coinObjects;
	ISceneNode* node = 0;
	ISceneNode* makeNode(ISceneManager* smgr, IVideoDriver* driver, vector3df pos);

	bool getEnemyStatus() {
		return enemy;
	}
	void setID(int ID) {
		id = ID;
	}
	void setMovingDirection(int moveDirection) {
		movingDirection = moveDirection;
		if (movingDirection > 0) node->setRotation(vector3df(0, -180, 0));
		else node->setRotation(vector3df(0, 0, 0));
	}
	float getMoveSpeed() {
		return moveSpeed;
	}
	void MoveObject(float frameDeltaTime) {
		if (moveSpeed != 0) {
			node->setPosition(vector3df(node->getPosition().X, node->getPosition().Y, node->getPosition().Z - moveSpeed * frameDeltaTime * movingDirection));
			if (node->getPosition().Z > 500) node->setPosition(vector3df(node->getPosition().X, node->getPosition().Y, -500));
			else if (node->getPosition().Z < -500) node->setPosition(vector3df(node->getPosition().X, node->getPosition().Y, 500));
		}
		if (id == Bomb) {
			node->setRotation(vector3df(node->getRotation().X, node->getRotation().Y + bombDistance*10*frameDeltaTime, node->getRotation().Z));
		}

		if (id == Coin) {
			node->setRotation(vector3df(node->getRotation().X, node->getRotation().Y + bombDistance * 10 * frameDeltaTime, node->getRotation().Z));

		}
	}
	void setNewObjPos(int movedirection, int newXpos, ISceneManager* smgr) {
		setMovingDirection(movedirection);
		node->setPosition(vector3df(newXpos, node->getPosition().Y, node->getPosition().Z));
		vector3df pos = node->getPosition();
		if (node->getID() == Bomb) {
			node->removeAnimators();
			ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(pos, pos + vector3df(0, 0, bombDistance), 120000 / bombDistance, true, true);
			node->addAnimator(anim);
			anim->drop();
		}
	}
	void spawnObjects(int id, float moveSpeed, int amount, ISceneManager* smgr, IVideoDriver* driver);
	void spawnCoins(int amount, ISceneManager* smgr, IVideoDriver* driver);


	void setExhaust(ISceneManager* smgr, IVideoDriver* driver);
	};


#endif // ! GAMEOBJECT_H

#pragma once
