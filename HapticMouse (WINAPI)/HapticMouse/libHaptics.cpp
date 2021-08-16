#include "libHaptics.h"

#include <chrono>
//#include <thread>
#include "shared.h"
#include "sharedInit.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include "interpol_rbf.h"
#include "read_data.h"

//------------------------------------------------------------------------------
using namespace std;
using namespace std::chrono;
//------------------------------------------------------------------------------

bool simulationIsRunning = true;

//cFrequencyCounter frequencyCounterVibroEstim;
//cFrequencyCounter frequencyCounterArRefresh;
double loopPeriodArRefresh = 1.0 / 100;   // 150 Hz

std::vector <float> outputHist[200];
std::vector <float> excitationHist[200];

boost::mt19937 rng;

/* Input variables for rendering lib
static double forceNormal = 2;
static int faceInContact;

static double magVel = 100;
static int mdl = 5;

// a flag that indicates if the haptic simulation is currently running
bool simulationRunning = false;

// a flag that indicates if the haptic simulation has terminated
bool simulationFinished = true;
*/

//void updateHaptics(void);
//void updateVibrationPattern(void);

// this function closes the application
//void WaitForLibClose(void);

std::map<int, std::wstring> InitPictures()
{
	std::map<int, std::wstring> MaterialPictures;
	
	for (int index = 1; index <= LIB_COUNT; index++)
	{
		wchar_t buffer[100];
		swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"material_%d.jpg", index);
		wstring fileName(buffer);
		MaterialPictures[index] = fileName;
	}

	//std::wstring material_name(L"paper.jpg");
	//MaterialPictures[Material::PAPER] = material_name;
	//material_name = L"wood.jpg";
	//MaterialPictures[Material::WOOD] = material_name;
	return MaterialPictures;
};

double LinearVelosity(int rotations, double time)
{
	double rad_angle = (rotations * ROTATION_ANGLE) / LOGITECH_STEPS_COUNT * RADIAN_ANGLE;
	double angle_velosity = rad_angle / time;
	double velosity = MOUSE_DIAMETER / 2 * angle_velosity;
	return velosity;
};

void LoadModel(string path ,int mdl)
{
	loadModels(path, mdl);
}


void WaitForLibClose(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { std::this_thread::sleep_for(100ms); }
	//while (true) { std::this_thread::sleep_for(100ms); }

}

static double vibrations(int mdl)
{
	double output = 0.0;
	double excitation = 0.0;
	double rgen_mean = 0.0;
	boost::mt19937 generator;

	//Double buffered, if buffer 1:
	if (SynthesisFlag_Buffer1[mdl]) {
		//generate Gaussian random number with power equal to interpolation model variance
		boost::normal_distribution<> nd(rgen_mean, sqrt(filtVariance_buf1[mdl]));
		boost::variate_generator<boost::mt19937&,
			boost::normal_distribution<> > var_nor(rng, nd);
		excitation = var_nor();
		output = 0.0;

		cout << excitation << endl;

		//if the size of output history is less than the number of AR coefficients, append zeros
		if (outputHist[mdl].size() < (unsigned int)MAX_COEFF) {
			int subt = MAX_COEFF - outputHist[mdl].size();
			for (int j = 0; j < subt; j++) {
				outputHist[mdl].push_back(0.0);
			}
		}
		//if the size of excitation history is less than the number of MA coefficients, append zeros
		if (excitationHist[mdl].size() < (unsigned int)MAX_MACOEFF) {
			int subt = MAX_MACOEFF - excitationHist[mdl].size();
			for (int j = 0; j < subt; j++) {
				excitationHist[mdl].push_back(0.0);
			}
		}

		//apply AR coefficients to history of output values
		for (int i = 0; i < coeffNum[mdl]; i++) {
			output += outputHist[mdl].at(i) * (-filtCoeff_buf1[mdl][i]);
		}

		output += excitation;

		//if the size of output history is greater than the number of AR coefficients, make the extra values zero so we're not storing junk
		if (outputHist[mdl].size() > (unsigned int) coeffNum[mdl]) {
			for (unsigned int kk = coeffNum[mdl]; kk < outputHist[mdl].size(); kk++)
				outputHist[mdl].at(kk) = 0.0;
		}
		//if the size of excitation history is greater than the number of MA coefficients, make the extra values zero so we're not storing junk
		if (excitationHist[mdl].size() > (unsigned int) MAcoeffNum) {
			for (unsigned int kk = MAcoeffNum; kk < excitationHist[mdl].size(); kk++)
				excitationHist[mdl].at(kk) = 0.0;
		}

	}
	else {//if buffer 2
		//generate Gaussian random number with power equal to interpolation model variance
		boost::normal_distribution<> nd(rgen_mean, sqrt(filtVariance_buf2[mdl]));
		boost::variate_generator<boost::mt19937&,
			boost::normal_distribution<> > var_nor(rng, nd);
		excitation = var_nor();
		output = 0.0;

		//if the size of output history is less than the number of AR coefficients, append zeros
		if (outputHist[mdl].size() < (unsigned int)MAX_COEFF) {
			int subt = MAX_COEFF - outputHist[mdl].size();
			for (int j = 0; j < subt; j++) {
				outputHist[mdl].push_back(0.0);
			}
		}
		//if the size of excitation history is less than the number of MA coefficients, append zeros
		if (excitationHist[mdl].size() < (unsigned int)MAX_MACOEFF) {
			int subt = MAX_MACOEFF - excitationHist[mdl].size();
			for (int j = 0; j < subt; j++) {
				excitationHist[mdl].push_back(0.0);
			}
		}

		//apply AR coefficients to history of output values
		for (int i = 0; i < coeffNum[mdl]; i++) {
			output += outputHist[mdl].at(i) * (-filtCoeff_buf2[mdl][i]);
		}

		//if applicable, also apply MA coefficients to history of excitation values
		output += excitation;


		//if the size of output history is greater than the number of AR coefficients, make the extra values zero so we're not storing junk
		if (outputHist[mdl].size() > (unsigned int) coeffNum[mdl]) {
			for (unsigned int kk = coeffNum[mdl]; kk < outputHist[mdl].size(); kk++) {
				outputHist[mdl].at(kk) = 0.0;
			}
		}
		//if the size of excitation history is greater than the number of MA coefficients, make the extra values zero so we're not storing junk
		if (excitationHist[mdl].size() > (unsigned int) MAcoeffNum) {
			for (unsigned int kk = MAcoeffNum; kk < excitationHist[mdl].size(); kk++)
				excitationHist[mdl].at(kk) = 0.0;
		}
	}

	// remove the last element of our output vector
	outputHist[mdl].pop_back();
	excitationHist[mdl].pop_back();
	// push our new ouput value onto the front of our vector stack
	outputHist[mdl].insert(outputHist[mdl].begin(), output);
	excitationHist[mdl].insert(excitationHist[mdl].begin(), excitation);

	return output; //this is the output vibration value (in m/s^2)
}

bool initial_touch = true;

void updateVibrationPattern(void) {
	while (simulationIsRunning)
	{
		high_resolution_clock::time_point start_time = high_resolution_clock::now();

		//int mdl=0;

		//double normForce = forceNormal;

		if (initial_touch) {
			initial_touch = false;
		}

		if (true) {

			if (std::abs(magVel) > 5) {
				ARFromLsf(magVel, forceNormal / 5.0, mdl); // Y and X are swaped due to difference modeling / redering coord sys.
			}
			else {
				filtVariance_buf1[mdl] = 0;
				filtVariance_buf2[mdl] = 0;
			}

		}
		else {
			filtVariance_buf1[mdl] = 0;
			filtVariance_buf2[mdl] = 0;
		}

		high_resolution_clock::time_point end_time = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);

		while (time_span.count() < loopPeriodArRefresh) {
			high_resolution_clock::time_point end_time
				= high_resolution_clock::now();

			time_span = duration_cast<duration<double>>(end_time - start_time);
		}
	}
}

void vibroOut(double vibroVal, int mdl)
{
	vibroVal *= (1.6 * gainConst[mdl]);
	if (vibroVal > 10.0)vibroVal = 10.0;
	if (vibroVal < -10.0)vibroVal = -10.0;

	vibrationVal = vibroVal;

	//cout << "Result: " << vibroVal << endl;
}

void updateHaptics(void)
{

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{

		// Haptic Texture Rendering
		//int mdl = (faceInContact>-1) ? object->mdlPerFace[faceInContact] : 0;

		int mdl = 5;

		if (mdl > 0) {
			double vibroOutput = vibrations(mdl);
			vibroOut(vibroOutput, mdl);
		}
		else {
			double vibroOutput = 0.0;
			vibroOut(vibroOutput, mdl);
		}
	}

	// exit haptics thread
	simulationFinished = true;
}