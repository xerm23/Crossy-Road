#include "Globals.h"
#include <iostream>
#include <fstream>
#include <string>

class GUIController {
private:
	int bestScore = 0;
	IrrlichtDevice* device;
	IGUIEnvironment* guienv;
	IGUIStaticText* currScoreTxt = 0;
	IGUIFont* newFont = 0;
	IGUISpriteBank* ButtonSprites = 0;
	IGUIButton* PlayBtn = 0;
	IGUIButton* SoundBtn = 0;
	IGUIButton* ExitBtn = 0;
	IGUIButton* nextBtn = 0;
	IGUIButton* prevBtn = 0;
	IGUIImage* modelImage = 0;
	float windowWidth;
	float windowHeight;

public:
	GUIController(IrrlichtDevice* devicee) {
		bestScore = getBestScore();
		device = devicee;
		guienv = device->getGUIEnvironment();
		windowWidth = device->getVideoModeList()->getDesktopResolution().Width;
		windowHeight = device->getVideoModeList()->getDesktopResolution().Height;
		newFont = guienv->getFont("Textures/font.bmp");
		ButtonSprites = guienv->addEmptySpriteBank("ButtonSprite");
		initButtons(false);
		initStartButtons();


	};

	//check if the current score is new best score
	void checkBestScore(int currentScore) {
		if (currentScore > getBestScore()) {
			std::ofstream myfile("PlayerPrefs/bestScore.txt");
			if (myfile.is_open()) {
				myfile << currentScore;
				myfile.close();
			}
			else std::cout << "Unable to open file";
		}
	}
	//pull best score from the text file
	int getBestScore() {
		std::string line;
		std::ifstream myfile("PlayerPrefs/bestScore.txt");
		if (myfile.is_open()){
			getline(myfile, line);
			myfile.close();
			return std::stoi(line);
		}
		else {
			std::cout << "Unable to open file";
			return 0;
		}
	}

	//add coin to the file
	void addCoin() {
		int coinsATM = getCoins();
		std::ofstream myfile("PlayerPrefs/coinsCollected.txt");
		if (myfile.is_open()) {
			myfile << coinsATM+1;
			myfile.close();
		}
		else std::cout << "Unable to open file";
		
	}

	int getCoins() {
		std::string line;
		std::ifstream myfile("PlayerPrefs/coinsCollected.txt");
		if (myfile.is_open()){
			getline(myfile, line);
			myfile.close();
			return std::stoi(line);
		}
		else {
			std::cout << "Unable to open file";
			return 0;
		}
	}
	//pull master sound volume from the text file
	int getSound() {
		std::string line;
		std::ifstream myfile("PlayerPrefs/soundOn.txt");
		if (myfile.is_open()) {
			getline(myfile, line);
			myfile.close();
			return std::stoi(line);
		}
		else {
			std::ofstream myfile("PlayerPrefs/soundOn.txt");
			if (myfile.is_open()) {
				myfile << 1;
				myfile.close();
				return 1;
			}
		}
	}
	void changeSound() {
		if (getSound() == 0) {
			std::ofstream myfile("PlayerPrefs/soundOn.txt");
			if (myfile.is_open()) {
				myfile << 1;
				myfile.close();
			}
			else {
				std::cout << "Unable to open soundOn.txt";
			}
		}
		else if (getSound() == 1) {
			std::ofstream myfile("PlayerPrefs/soundOn.txt");
			if (myfile.is_open()) {
				myfile << 0;
				myfile.close();
			}
			else {
				std::cout << "Unable to open soundOn.txt";
			}		
		}
	}
	
	//show scores on top left corner
	void showScores(int currentScore) {
		guienv->clear();
		currScoreTxt = guienv->addStaticText(L"",
			rect<s32>(50, 20, 450, 300), false, false, 0, -1, false);
		currScoreTxt->setOverrideFont(newFont);
		std::wstring wide_string = L"Current Score: " + std::to_wstring(currentScore)
			+ L"\nBest Score: " + std::to_wstring(getBestScore())
			+ L"\nCoins Collected: " + std::to_wstring(getCoins());
		const wchar_t* result = wide_string.c_str();
		currScoreTxt->setText(result);

		device->getVideoDriver()->draw2DRectangle(
			SColor(125, 255, 255, 255), rect<s32>(20, 20, 350, 150), 0);

		guienv->drawAll();
	}

	//this function should be called when the game is over
	void showGameOver() {
		guienv->clear();
		IGUIButton* qwe = guienv->addButton(rect<s32>(0, 0, windowWidth, windowHeight), 0, -1, L"", 0);
		qwe->setOverrideFont(newFont);
		qwe->setText(L"Press Enter to retry!");
		device->getVideoDriver()->draw2DRectangle(SColor(255, 255, 255, 255), rect<s32>(0, 0, windowWidth, windowHeight), 0);
		guienv->drawAll();

	}

	void initButtons(bool isPaused) {
		ButtonSprites->addTextureAsSprite(device->getVideoDriver()->getTexture("Textures/Water.jpg"));
		ButtonSprites->addTextureAsSprite(device->getVideoDriver()->getTexture("Textures/Grass.jpg"));

		PlayBtn = guienv->addButton(
			rect<s32>(windowWidth / 2 - 100, windowHeight / 2 - 150,
					windowWidth / 2 + 100, windowHeight / 2 + -70), 0,
					BTN_PLAY, L"", 0);
		PlayBtn->setOverrideFont(newFont);
		if(isPaused)
		PlayBtn->setText(L"Resume");
		else PlayBtn->setText(L"Play game.");

		PlayBtn->setSpriteBank(ButtonSprites);
		PlayBtn->setSprite(EGBS_BUTTON_MOUSE_OFF, 0);
		PlayBtn->setSprite(EGBS_BUTTON_MOUSE_OVER, 1);


		SoundBtn = guienv->addButton(
			rect<s32>(windowWidth / 2 - 100, windowHeight / 2 -50,
					  windowWidth / 2 + 100, windowHeight / 2 + 30), 0,
					  BTN_SOUND, L"", 0);
		SoundBtn->setSpriteBank(ButtonSprites);
		SoundBtn->setSprite(EGBS_BUTTON_MOUSE_OFF, 0);
		SoundBtn->setSprite(EGBS_BUTTON_MOUSE_OVER, 1);

		SoundBtn->setOverrideFont(newFont);
		if (getSound() == 1)
			SoundBtn->setText(L"Sound: On");
		else
			SoundBtn->setText(L"Sound: Off");

		ExitBtn = guienv->addButton(
			rect<s32>(windowWidth / 2 - 100, windowHeight / 2 + 50,
					  windowWidth / 2 + 100, windowHeight / 2 + 130), 0,
					  BTN_EXIT, L"", 0);

		ExitBtn->setSpriteBank(ButtonSprites);
		ExitBtn->setSprite(EGBS_BUTTON_MOUSE_OFF, 0);
		ExitBtn->setSprite(EGBS_BUTTON_MOUSE_OVER, 1);

		ExitBtn->setOverrideFont(newFont);
		ExitBtn->setText(L"Exit");

	}

	void initStartButtons() {
		prevBtn = guienv->addButton(rect<s32>(
			windowWidth - 600, windowHeight / 2 - 50,
			windowWidth - 530, windowHeight / 2 + 30), 0,
			BTN_PREV, L"", 0);
		prevBtn->setOverrideFont(newFont);
		prevBtn->setText(L"<");

		prevBtn->setSpriteBank(ButtonSprites);
		prevBtn->setSprite(EGBS_BUTTON_MOUSE_OFF, 0);
		prevBtn->setSprite(EGBS_BUTTON_MOUSE_OVER, 1);

		nextBtn = guienv->addButton(rect<s32>(
			windowWidth - 100, windowHeight / 2 - 50,
			windowWidth - 30, windowHeight / 2 + 30), 0,
			BTN_NEXT, L"", 0);
		nextBtn->setOverrideFont(newFont);
		nextBtn->setText(L">");

		nextBtn->setSpriteBank(ButtonSprites);
		nextBtn->setSprite(EGBS_BUTTON_MOUSE_OFF, 0);
		nextBtn->setSprite(EGBS_BUTTON_MOUSE_OVER, 1);

		modelImage = guienv->addImage(rect<s32>(
			windowWidth - 500, windowHeight / 2 - 250,
			windowWidth - 130, windowHeight / 2 + 250), 0,
			0, L"", true);
		modelImage->setScaleImage(true);
		modelImage->setImage(device->getVideoDriver()->getTexture("Textures/Chick.png"));
	}

	//this function should be called before the game has started
	void showHomeScreen() {
		if (getSound() == 1)
			SoundBtn->setText(L"Sound: On");
		else
			SoundBtn->setText(L"Sound: Off");

		device->getVideoDriver()->draw2DRectangle(SColor(255, 200, 200, 155), rect<s32>(0, 0, windowWidth, windowHeight), 0);
		guienv->drawAll();

	}

	void showPauseScreen() {
		if (getSound() == 1)
			SoundBtn->setText(L"Sound: On");
		else
			SoundBtn->setText(L"Sound: Off");
		device->getVideoDriver()->draw2DRectangle(SColor(120, 200, 200, 155), rect<s32>(0, 0, windowWidth, windowHeight), 0);
		guienv->drawAll();

	}

	void changeModelImage(int modelID) {
		if (modelID == 0) {
			modelImage->setImage(device->getVideoDriver()->getTexture("Textures/Chick.png"));
		}
		if (modelID == 1) {
			modelImage->setImage(device->getVideoDriver()->getTexture("Textures/Penguin.png"));
		}
		if (modelID == 2) {
			modelImage->setImage(device->getVideoDriver()->getTexture("Textures/Pig.png"));
		}

	}


};


