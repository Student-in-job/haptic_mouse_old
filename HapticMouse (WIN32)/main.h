#pragma once

#define LOGITECH_STEPS_COUNT	48		// Number of steps in one 360 degree rotation of mouse wheel
#define ROTATION_ANGLE			360		// degrees in full rotation
#define RADIAN_ANGLE			57.2958	// Radian to angle conversion
#define MOUSE_DIAMETER			2.48	// mouse wheel diameter in cm

//--------- Window ---------
int windowWidth = 470;
int windowHeight = 600;
int margin;
int commonWidth;
int commonHeight;
//--------- Haptic model image ---------
int imageX;
int imageY;
int imageWidth;
int imageHeight;
//--------- Port Label ---------
int staticPortX;
int staticPortY;
int staticPortWidth;
int staticPortHeight;

int portLabelX;
int portLabelY;
int portLabelWidth;
int portLabelHeight;
//--------- Model label ---------
int staticModelX;
int staticModelY;
int staticModelWidth;
int staticModelHeight;

int modelLabelX;
int modelLabelY;
int modelLabelWidth;
int modelLabelHight;
//--------- Velocity label ---------
int staticVelocityX;
int staticVelocityY;
int staticVelocityWidth;
int staticVelocityHeight;

int velocityLabelX;
int velocityLabelY;
int velocityLabelWidth;
int velocityLabelHeight;
//--------- Panel ---------
int panelY;
int panelX;
int panelWidth;
int panelHeight;
//--------- Haptic model combobox ---------
int comboModelX;
int comboModelY;
int comboModelWidth;
int comboModelHeight;
//--------- Start Button ---------
int startButtonX;
int startButtonY;
int startButtonWidth;
int startButtonHeight;
//--------- Stop Button ---------
int stopButtonX;
int stopButtonY;
int stopButtonWidth;
int stopButtonHeight;