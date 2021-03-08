#ifndef GLOBALS_h
#define GLOBALS_h

#include <irrlicht.h>
#include <stdio.h>
#include <irrKlang.h>
#include <iostream>
#include <random>
#include <chrono>
#include <cmath>
#include <stdlib.h>     /* srand, rand */


// include console I/O methods (conio.h for windows, our wrapper in linux)
#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

enum NodeIDs{
	GrassRow = 0,
	WaterRow = 1,
	RoadRow = 2,
	Log = 3,
	Car = 4,
	Player = 5,
	Hawk = 6,
	Bomb = 7,
	Coin = 8
};

enum windowSize {
	windowWidth = 3840,
	windowHeight = 2160
};

enum movingDirection {
	left = -1,
	right = 1
};

enum soundEnabled {
	soundOff = 0,
	soundOn = 1
};

enum GUIButtons {
	BTN_PLAY,
	BTN_SOUND,
	BTN_EXIT,
	BTN_NEXT,
	BTN_PREV
};

#endif