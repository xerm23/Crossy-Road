#include "Game.h"

std::random_device dev;
std::mt19937 rng(dev());
//random row generator based on row ID
std::uniform_int_distribution<std::mt19937::result_type> randomRow(0, 2);
//0 - grass
//1 - water
//2 - road

int currentScore = 0;

bool hawkAttack = false;
bool hawkSpawned = false;

float speedMultiplier = 1;


void Game::rowsInit() {
	//init player
	chick = new GameObject(Player, currentModel);
	//chick->modelID = playerModelID;
	chick->makeNode(getManager(), getDriver(), vector3df(0, 25, 0));

	//camera
	mainCamera = getManager()->addCameraSceneNode();
	mainCamera->setPosition(camPos);
	mainCamera->setTarget(camTarget);

	//lightning
	ILightSceneNode* myLight = getManager()->addLightSceneNode();
	myLight->setPosition(vector3df(20, 5.0f, -25));
	SLight lightData;
	lightData.AmbientColor = SColor(255, 255, 255, 255);
	myLight->setLightData(lightData);
	myLight->setParent(mainCamera);
	getManager()->setShadowColor(video::SColor(100, 0, 0, 0));

	//hawk
	hawk = new GameObject(Hawk);
	hawk->makeNode(getManager(), getDriver(), vector3df(-17, 0, 55));
	hawk->node->setParent(mainCamera);



	for (int i = -10; i < 60; i++)
	{
		GameObject* row = new GameObject();

		int temp = randomRow(rng);
		int movingDirectionTemp = rand() % 2;
		if (i < 1) {
			row->setID(GrassRow);
			row->makeNode(getManager(), getDriver(), vector3df(i * 10, 0, 0));
			row->node->setPosition(vector3df(i * 10, 0, 0));
		}

		else {
			switch (temp) {
			case 0:
				row->setID(GrassRow);
				row->makeNode(getManager(), getDriver(), vector3df(i * 10, 0, 0));
				row->spawnCoins(4, getManager(), getDriver());
				break;
			case 1:
				row = new GameObject(WaterRow);
				row->makeNode(getManager(), getDriver(), vector3df(i * 10, 0, 0));
				if (rowsActive.getLast()->movingDirection == right)
					row->movingDirection = left;
				else
					row->movingDirection = right;
				row->spawnObjects(Log, logSpeed, 12, getManager(), getDriver());

				break;
			case 2:
				row->setID(RoadRow);
				row->makeNode(getManager(), getDriver(), vector3df(i * 10, 0, 0));
				if (rowsActive.getLast()->movingDirection == right)
					row->movingDirection = left;
				else
					row->movingDirection = right;
				row->spawnObjects(Car, carSpeed, 8, getManager(), getDriver());

				break;

			default:
				break;
			}
		}
		rowsActive.push_back(row);
	}
}
bool Game::init() {
	//Irrlicht setup and other init stuff
	device = createDevice(EDT_OPENGL, dimension2d<u32>(windowWidth, windowHeight), 32, true, false, false);
	if (!device) return false;
	device->setEventReceiver(this);
	device->maximizeWindow();

	IFileSystem* filesystem = getManager()->getFileSystem();
	filesystem->changeWorkingDirectoryTo("Assets");

	soundEngine = createIrrKlangDevice();
	if (!soundEngine)
	{
		printf("Could not startup sound engine\n");
		return 0; // error starting up the engine
	}
	//windows title
	device->setWindowCaption(L"Crossy Road");

	soundEngine->play2D("Audio/music.wav", true);
	//rows 
	rowsInit();

	//gui
	guiController = new GUIController(device);

	gameOver = false;


	return true;

}

void Game::run() {
	u32 then = device->getTimer()->getTime();
	u32 hawkTimeThen = device->getTimer()->getTime();
	bool onlog = false;

	//gamelogic and renderloop dependent on game state
	while (device->run()){
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		//hawkDelta is used for showHawkMeter() timer and 250 should trigger the hawkAttack bool
		const u32 hawkTimeNow = device->getTimer()->getTime();
		const f32 hawkDelta = (f32)(hawkTimeNow - hawkTimeThen) / 15.f;

		if (device->isWindowActive()) {
			//gravity reset
			float gravity = 55;

			//iteration through rows
			for (int i = 0; i < rowsActive.size(); i++) {
				GameObject* row = rowsActive[i];
				if (collision(chick->node, row->node)) {
					gravity = 0;
					if (row->getEnemyStatus()) {
						if (!soundEngine->isCurrentlyPlaying("Audio/hit.wav")&& !start && !gameOver){
							soundEngine->play2D("Audio/hit.wav");
						}
						//game over!!!
						gameOver = true;
					}
				}
				//iteration through objects on row
				for (int j = 0; j < row->rowObjects.size(); j++) {
					GameObject* objOnRow = row->rowObjects[j];
					if (collision(chick->node, objOnRow->node)) {
						if (objOnRow->node->getID() == Log) {
							onlog = true;
							gravity = 0;
							chickOnLogSpeed = -objOnRow->getMoveSpeed() * objOnRow->movingDirection;
						}
						if (objOnRow->getEnemyStatus()) {
							if (!soundEngine->isCurrentlyPlaying("Audio/hit.wav") && !start && !gameOver) {
								soundEngine->play2D("Audio/hit.wav");
							}
							//game over!!!
							gameOver = true;
						}
					}
					objOnRow->MoveObject(frameDeltaTime*speedMultiplier);
				}

				//iterate through coins
				for (size_t p = 0; p < row->coinObjects.size(); p++)
				{
					GameObject* objOnRow = row->coinObjects[p];
					if (collision(chick->node, objOnRow->node) && !start) {
						getManager()->addToDeletionQueue(objOnRow->node);
						row->coinObjects.erase(p);
						guiController->addCoin();
						if (!soundEngine->isCurrentlyPlaying("Audio/coin.wav")) {
							soundEngine->play2D("Audio/coin.wav");
						}
					}
					objOnRow->MoveObject(frameDeltaTime);
				}
			}

			//jumped on moving log in water
			if (onlog) {
				chick->node->setPosition(vector3df(chick->node->getPosition().X, chick->node->getPosition().Y, chick->node->getPosition().Z + chickOnLogSpeed * frameDeltaTime*speedMultiplier));
			}


			//move rows to inactive
			if (chick->node->getPosition().X > rowsActive[0]->node->getPosition().X + 200) {
				rowsInactive.push_back(rowsActive[0]);
				rowsActive.erase(0);
			}


			//move inactive row to active
			if (chick->node->getPosition().X > rowsActive.getLast()->node->getPosition().X - 300) {
				vector3df newPos = rowsActive.getLast()->node->getAbsolutePosition();
				newPos.X += 10;
				//pick a random inactive row
				std::uniform_int_distribution<std::mt19937::result_type> randomInactiveRow(0, rowsInactive.size() - 1);
				int rowIndex = randomInactiveRow(rng);
				GameObject* rowToMove = rowsInactive[rowIndex];
				if (rowsActive.getLast()->movingDirection == right)
					rowToMove->movingDirection = left;
				else
					rowToMove->movingDirection = right;
				rowToMove->node->setPosition(newPos);
				rowsActive.push_back(rowToMove);

				//move objects on the row
				for (size_t i = 0; i < rowsActive.getLast()->rowObjects.size(); i++)
					rowsActive.getLast()->rowObjects[i]->setNewObjPos(rowToMove->movingDirection, newPos.X, getManager());

				//spawn bombs on water row
				if (rowToMove->node->getID() == GrassRow && rowToMove->rowObjects.size() == 0) {
					rowToMove->spawnObjects(Bomb, 0, 7, getManager(), getDriver());
					rowToMove->spawnCoins(4, getManager(), getDriver());

				}

				rowsInactive.erase(rowIndex);
			}

			//game over? press enter to retry
			if (gameOver) {
				if (isKeyDown(KEY_RETURN)) {
					gameOver = false;
					start = false;
					device->getCursorControl()->setVisible(false);
					restart();
				}
			}

			//hawk bar is full -> hawk attack/game over
			if (hawkDelta > 250) {
				hawk->node->grab();
				hawk->node->setParent(getManager()->getRootSceneNode());
				vector3df hawkPos = mainCamera->getPosition() + hawk->node->getPosition();
				hawk->node->setPosition(hawkPos);
				hawkAttack = true;
				hawkTimeThen = hawkTimeNow;


			}

			//hawk enters attack mode
			if (hawkAttack) {
				vector3df hawkPos = hawk->node->getAbsolutePosition();
				vector3df hawkMoveDirection = vector3df(chick->node->getPosition() - hawkPos);
				hawkPos += hawkMoveDirection * frameDeltaTime * 3;
				hawk->node->setPosition(hawkPos);
				if (collision(hawk->node, chick->node)) {
					if (!soundEngine->isCurrentlyPlaying("Audio/hit.wav") && !start && !gameOver) {
						soundEngine->play2D("Audio/hit.wav");
					}
					gameOver = true;
				}
			}

			//falling from left or right edge of row
			if (chick->node->getAbsolutePosition().Y < -2) {
				gameOver = true;
			}

			//check input
			if (gravity == 0 && !gameOver && !hawkAttack && !paused) {
				if (isKeyDown(KEY_UP)) {
					chick->node->setPosition(vector3df(chick->node->getPosition().X + 10, chick->node->getPosition().Y + 10, chick->node->getPosition().Z));
					chickRot = vector3df(0, 0, 0);
					if (chick->node->getPosition().X / 10 > currentScore) {
						currentScore += 1;
						hawkTimeThen = hawkTimeNow;
					}
					guiController->checkBestScore(currentScore);
				}
				if (isKeyDown(KEY_DOWN) && chick->node->getPosition().X > 1) {
					chick->node->setPosition(vector3df(chick->node->getPosition().X - 10, chick->node->getPosition().Y + 10, chick->node->getPosition().Z));
					chickRot = vector3df(0, -180, 0);
				}
				if (isKeyDown(KEY_LEFT)) {
					chick->node->setPosition(vector3df(chick->node->getPosition().X, chick->node->getPosition().Y + 10, chick->node->getPosition().Z + 10 + speedMultiplier*abs(chickOnLogSpeed) / 10));
					chickRot = vector3df(0, -90, 0);
				}
				if (isKeyDown(KEY_RIGHT)) {
					chick->node->setPosition(vector3df(chick->node->getPosition().X, chick->node->getPosition().Y + 10, chick->node->getPosition().Z - 10 - speedMultiplier*abs(chickOnLogSpeed) / 10));
					chickRot = vector3df(0, 90, 0);
				}
			}

			onlog = false;
			chickOnLogSpeed = 0;

			//camera movement
			vector3df camMoveDirection = vector3df(chick->node->getPosition().X - camTarget.X, 0, chick->node->getPosition().Z - camTarget.Z);
			camPos += camMoveDirection * frameDeltaTime;
			camTarget += camMoveDirection * frameDeltaTime;
			mainCamera->setPosition(camPos);
			mainCamera->setTarget(camTarget);

			//chick movement
			chick->node->setPosition(chick->node->getPosition() + vector3df(0, -gravity * frameDeltaTime, 0));
			chick->node->setRotation(chickRot);

			getDriver()->beginScene(true, true, SColor(255, 50, 50, 50));
			getManager()->drawAll();
			if(gameOver) {
				chick->node->setPosition(vector3df(0, 20, 0));
				hawkTimeThen = hawkTimeNow;
				if (start) {
					guiController->showHomeScreen();
				}
				else
					guiController->showGameOver();
			}
			if(!start && !paused)
			guiController->showScores(currentScore);

			if (paused) {
				guiController->showPauseScreen();
			}

			//spawnHawkHere
			if (currentScore > 5) {
				if(!hawkAttack && !gameOver)
				showHawkMeter(getDriver(), hawkDelta);
				if (hawkSpawned == false) {
					hawkSpawned = true;
					soundEngine->play2D("Audio/HawkSpawn.wav", false);
				}
				else {
					if(hawk->node->getAbsolutePosition().Y > 30)
						hawk->node->setPosition(hawk->node->getPosition() - vector3df(0, frameDeltaTime*75, 0));					
				}
			}
			//no hawk? - reset timer
			else {
				hawkTimeThen = hawkTimeNow;
			}

			//sound engine master volume
			soundEngine->setSoundVolume(guiController->getSound());

			//make cars faster 
			if (currentScore > 100) speedMultiplier = currentScore / 100.0;
			else speedMultiplier = 1;
			getDriver()->endScene();


		}
		else device->yield();
	}
}

//reset the scene 
void Game::restart()
{
	hawkAttack = false;
	hawkSpawned = false;
	delete hawk;

	rowsActive.clear();
	rowsInactive.clear();
	getManager()->clear();

	rowsInit();
	currentScore = 0;
}
