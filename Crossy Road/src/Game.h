#ifndef GAME_H
#define GAME_H

#include "GameObject.h"
#include "GUIController.h"


class Game : public IEventReceiver{
private:
	bool start = true;
	bool gameOver = false;

	bool paused = false;
	int coinsCollected = 0;

	//0 chick, 1 penguin, 2 pig
	int currentModel = 0;


	array<GameObject*> rowsActive;
	array<GameObject*> rowsInactive;

	IrrlichtDevice* device;
	ISoundEngine* soundEngine;;
	bool KeyDown[KEY_KEY_CODES_COUNT];
	bool KeyUp[KEY_KEY_CODES_COUNT];

	//GUI
	GUIController* guiController;

	//player
	GameObject* chick = 0;
	vector3df chickRot = vector3df(0, 0, 0);

	GameObject* hawk = 0;
	bool hawkSpawned = false;
	bool hawkAttack = false;

	//camera
	ICameraSceneNode* mainCamera;
	vector3df camPos = vector3df(-30, 100, -15);

	vector3df camTarget = vector3df(0, 30, 0);

	float logSpeed = 35;
	float chickOnLogSpeed = 0;
	float carSpeed = 50;


	bool collision(ISceneNode* sn1, ISceneNode* sn2) {
		return sn1->getTransformedBoundingBox().intersectsWithBox(sn2->getTransformedBoundingBox());
	}

public:
	//constructor
	Game() {

		device = 0;
		soundEngine = 0;
		chick = 0;
		for (int i = 0; i < KEY_KEY_CODES_COUNT; i++) {
			KeyDown[i] = false;
			KeyUp[i] = true;
		}
	}
	//destructor
	~Game() {
		if (device) device->drop(); //drop the device, otherwise memory leaks will occur!
	}
	//init all rows
	void rowsInit();
	//init game
	bool init();
	//main game loop
	void run();

	//restart game after dying
	void restart();


	//show bar when Hawk comes
	void showHawkMeter(IVideoDriver* driver, float time) {
		float windowHeight = device->getVideoModeList()->getDesktopResolution().Height;
		driver->draw2DRectangle(SColor(255, time, 0, 255-time), rect<s32>(25, windowHeight - 50, 25+time, windowHeight-10), 0);
		driver->draw2DRectangleOutline(rect<s32>(25, windowHeight - 50, 275, windowHeight-10), SColor(255, 0, 0, 0));
	}

	inline IrrlichtDevice* getDevice() const { return device; }
	inline ISceneManager* getManager() const { return device->getSceneManager(); }
	inline IVideoDriver* getDriver() const { return device->getVideoDriver(); }

	//Input check
	struct SMouseState
	{
		core::position2di Position;
		bool LeftButtonDown;
		SMouseState() : LeftButtonDown(false) { }
	} MouseState;


	virtual bool OnEvent(const SEvent& event){
		switch (event.EventType){
		case EET_KEY_INPUT_EVENT: {
			if (KeyUp[KEY_ESCAPE]) {
				if (event.KeyInput.Key == KEY_ESCAPE && !start && !gameOver) {

					paused = !paused;
					device->getCursorControl()->setVisible(paused);
					if (paused) {
						device->getTimer()->stop();
						guiController->initButtons(true);
					}			
					else device->getTimer()->start();
				}
			}

			KeyDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
			KeyUp[event.KeyInput.Key] = !event.KeyInput.PressedDown;
			break;
		}
		default:
			break;
		}
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT){
			switch (event.MouseInput.Event){
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;
			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;
			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;
			default:
				// We won't use the wheel
				break;
			}
		}

		if (event.EventType == irr::EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* guienv = device->getGUIEnvironment();
			if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			{
				if (id == BTN_PLAY){
					start = false;
					gameOver = false;
					coinsCollected = 0;
					device->getCursorControl()->setVisible(false);
					if (paused) {
						paused = false;
						device->getTimer()->start();
					}
					else {
						getManager()->addToDeletionQueue(chick->node);
						chick = new GameObject(Player, currentModel);
						chick->makeNode(getManager(), getDriver(), vector3df(0, 25, 0));
					}					
					return true;
				}

				if (id == BTN_SOUND){
					guiController->changeSound();
					return true;
				}
				if (id == BTN_EXIT){
					device->closeDevice();
					return true;
				}
				if (id == BTN_NEXT){
					currentModel++;
					if (currentModel > 2) currentModel = 2;
					guiController->changeModelImage(currentModel);

					return true;
				}
				if (id == BTN_PREV){
					currentModel--;
					if (currentModel < 0) currentModel = 0;
					guiController->changeModelImage(currentModel);
					return true;
				}
			}
		}
		return false;
	}

	virtual bool isKeyDown(EKEY_CODE keyCode) const{
		return KeyDown[keyCode];
	}

	virtual bool isKeyUp(EKEY_CODE keyCode) const{
		return KeyUp[keyCode];
	}
	const SMouseState& GetMouseState(void) const{
		return MouseState;
	}
};
#endif // !GAME_H
#pragma once
