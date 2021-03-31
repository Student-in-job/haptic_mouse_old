#pragma once

#include <map>
#include <string>

#define LOGITECH_STEPS_COUNT	48		// Number of steps in one 360 degree rotation of mouse wheel
#define ROTATION_ANGLE			360		// degrees in full rotation
#define RADIAN_ANGLE			57.2958	// Radian to angle conversion
#define MOUSE_DIAMETER			2.48	// mouse wheel diameter in cm

// TODO: place all materials
enum Material
{
	PAPER = 20001,
	WOOD  = 20002
};


struct FrequencyRange {
	double min;
	double max;
};

static class Velosity
{
	public:
		/// <summary>
		/// Returns Linear velosity
		/// </summary>
		/// <param name="rotations"></param>
		/// <returns></returns>
		static double LinearVelosity(int rotations, double time);
};

class VibroFeedback
{
	public:
		std::map<int, FrequencyRange> materials;
		std::map<int, std::wstring> MaterialPictures;
		VibroFeedback();
	private:
		void InitVibrofeedbackRange();
};