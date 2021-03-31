#include "libHaptics.h"

double Velosity::LinearVelosity(int rotations, double time)
{
	double rad_angle = (rotations * ROTATION_ANGLE) / LOGITECH_STEPS_COUNT * RADIAN_ANGLE;
	double angle_velosity = rad_angle / time;
	double velosity = MOUSE_DIAMETER / 2 * angle_velosity;
	return velosity;
}

void VibroFeedback::InitVibrofeedbackRange()
{
	// TODO: Init frequency range for each material (in future will use a dataset)
	FrequencyRange paper = {300 , 60};
	this->materials[Material::PAPER] = paper;
	
	// Init the names of pictures
	std::wstring material_name(L"paper.jpg");
	this->MaterialPictures[Material::PAPER] = material_name;
	material_name = L"wood.jpg";
	this->MaterialPictures[Material::WOOD] = material_name;
}

VibroFeedback::VibroFeedback()
{
	this->InitVibrofeedbackRange();
}
