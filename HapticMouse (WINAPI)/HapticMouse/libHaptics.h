#pragma once

#include <map>
#include <string>
#include <thread>

#define LOGITECH_STEPS_COUNT	48		// Number of steps in one 360 degree rotation of mouse wheel
#define ROTATION_ANGLE			360		// degrees in full rotation
#define RADIAN_ANGLE			57.2958	// Radian to angle conversion
#define MOUSE_DIAMETER			2.48	// mouse wheel diameter in cm

void WaitForLibClose(void);

// TODO: place all materials
enum Material
{
	PAPER = 20001,
	WOOD  = 20002
};

std::map<int, std::wstring> InitPictures();
double LinearVelosity(int rotations, double time);

void LoadModel(std::string path, int mdl);
void updateHaptics(void);
void updateVibrationPattern(void);