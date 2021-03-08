#include "GameObject.h"


std::random_device rand_device;
std::mt19937 rngg(rand_device());

ISceneNode* GameObject::makeNode(ISceneManager* smgr, IVideoDriver* driver, vector3df pos) {

	if (id == GrassRow) {
		node = smgr->addCubeSceneNode();
		node->setMaterialTexture(0, driver->getTexture("Textures/Grass.jpg"));
		node->setScale(vector3df(1, 1, 100));
	}
	if (id == RoadRow) {
		node = smgr->addCubeSceneNode();
		node->setMaterialTexture(0, driver->getTexture("Textures/Road.jpg"));
		node->setScale(vector3df(1, 1, 100));
	}
	if (id == WaterRow) {
		node = smgr->addCubeSceneNode();
		node->setMaterialTexture(0, driver->getTexture("Textures/Water.jpg"));
		node->setScale(vector3df(1, 1, 100));
		enemy = true;
	}
	if (id == Log) {
		node = smgr->addCubeSceneNode();
		node->setMaterialTexture(0, driver->getTexture("Textures/Log.jpg"));
		node->setScale(vector3df(.7, .1f, 3.5f));
	}
	if (id == Car) {
		enemy = true;
		node = smgr->addEmptySceneNode();
		node->setScale(vector3df(3, 3, 4));
		setExhaust(smgr, driver);
		IMesh* carMesh = smgr->getMesh("Models/Car/Car.obj");
		ISceneNode* node2 = smgr->addMeshSceneNode(carMesh);
		node2->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);

		node2->setParent(node);

		if (movingDirection > 0) node->setRotation(vector3df(0, 180, 0));
	}
	if (id == Player) {
		node = smgr->addEmptySceneNode();
		IMesh* chickMesh = smgr->getMesh("Models/Chick/Chick.obj");
		if(modelID == 1)		
		chickMesh = smgr->getMesh("Models/Penguin/Penguin.obj");
		if(modelID == 2)		
		chickMesh = smgr->getMesh("Models/Pig/Pig.obj");
		IMeshSceneNode* meshNode = smgr->addMeshSceneNode(chickMesh);
		meshNode->addShadowVolumeSceneNode();
		meshNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
		meshNode->setParent(node);
		node->setScale(vector3df(2.f, 2.5f, 2.5f));
	}
	if (id == Hawk) {
		IMesh* hawkMesh = smgr->getMesh("Models/Hawk/Hawk.obj");
		node = smgr->addMeshSceneNode(hawkMesh);
		node->setRotation(vector3df(0, 90, 25));
		node->setScale(vector3df(10, 10, 10));
	}
	if (id == Bomb) {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> distrib(30, 70);
		bombDistance = distrib(dev);
		IMesh* bombMesh = smgr->getMesh("Models/Bomb/Bomb.obj");
		node = smgr->addMeshSceneNode(bombMesh);
		ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(pos- vector3df(0, 10, 0), pos + vector3df(0, -10, bombDistance), 120000/bombDistance, true, true);
		node->addAnimator(anim);
		anim->drop();
		node->setScale(vector3df(8, 8, 8));
		enemy = true;	  
	}
	if (id == Coin) {
		IMesh* coinMesh = smgr->getMesh("Models/Coin/Coin.obj");
		node = smgr->addMeshSceneNode(coinMesh);
		node->setScale(vector3df(2, 2, 2));
		node->setMaterialFlag(EMF_LIGHTING, false);
		bombDistance = 40;
	}
	node->setPosition(pos);
	node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	node->setID(id);
	return node;

}

//sets exhaust particles for car
void GameObject::setExhaust(ISceneManager* smgr, IVideoDriver* driver) {
	IParticleSystemSceneNode* ps = 0;
	ps = smgr->addParticleSystemSceneNode(false);

	IParticleEmitter* em = ps->createCylinderEmitter(
		vector3df(0, 0, 0), .05f, //center & radius
		vector3df(0, 1, 0), 2.f, // normal & length
		false, // outline only
		vector3df(0.0f, 0.0f, -0.03f), //initial direction
		20, 50, //min & max emit rate
		SColor(255, 0, 0, 0), //start color
		SColor(255, 255, 255, 255), //end color
		40, 60, 10, // min and max age, angle
		dimension2df(2, 2), //min size
		dimension2df(4, 4)); //max size

	ps->setEmitter(em); // set the emitter to particle system
	em->drop(); // now we can drop emitter

	ps->setPosition(vector3df(0, -.5f, -2.5));
	ps->setMaterialFlag(EMF_LIGHTING, false);
	ps->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, driver->getTexture("Textures/smokeparticle.png"));
	ps->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);

	IParticleAffector* pa = ps->createFadeOutParticleAffector();
	pa = ps->createGravityAffector(vector3df(0, 1, 0));
	ps->addAffector(pa);
	pa->drop();
	ps->setParent(node);


}


void GameObject::spawnObjects(int id, float moveSpeed,
	int amount, ISceneManager* smgr, IVideoDriver* driver) {
	float width = node->getScale().Z * 10;
	float interval = width / amount;

	std::uniform_int_distribution<std::mt19937::result_type> distrib(0, interval / 2);

	int randomOffset = distrib(rand_device) - interval / 4;

	for (int i = -amount / 2; i < amount / 2; i++)
	{
		GameObject* obj = new GameObject(id, moveSpeed);
		obj->movingDirection = movingDirection;
		obj->makeNode(smgr, driver, vector3df(node->getPosition().X, 5, interval * i + randomOffset));
		rowObjects.push_back(obj);
	}
}



void GameObject::spawnCoins(int amount, ISceneManager* smgr, IVideoDriver* driver) {
	float width = node->getScale().Z * 10;

	for (size_t i = 0; i < coinObjects.size(); i++)
		smgr->addToDeletionQueue(coinObjects[i]->node);

	coinObjects.clear();

	for (int i = -amount / 2; i < amount / 2; i++)
	{
		std::uniform_int_distribution<std::mt19937::result_type> distrib(0, width);
		int randomOffset = distrib(rand_device) - width / 2;

		GameObject* obj = new GameObject(Coin);
		obj->movingDirection = movingDirection;
		obj->makeNode(smgr, driver, vector3df(node->getPosition().X, 5, randomOffset));
		coinObjects.push_back(obj);
	}

}
