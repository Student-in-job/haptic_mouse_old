#include "shared.h"
#include <fstream>
#include <string>

void readData(int mdl);
void hapticTextureVerteces();

void loadModels(){
	bool mdl_list[200];
	
    for (int i = 0; i < 200; i++)
		mdl_list[i] = false;
    
    // Including the models

	mdl_list[4] = true;
	mdl_list[5] = true;
	mdl_list[6] = true;
	mdl_list[18] = true;
	mdl_list[27] = true;
	mdl_list[28] = true;
	mdl_list[50] = true;
	mdl_list[89] = true;
	mdl_list[92] = true;
	mdl_list[96] = true;

    //\ Including the models
	for (int i = 0; i < 200; i++){
		if (!mdl_list[i]) continue;
		else readData(i);
	}

	hapticTextureVerteces();

}

void hapticTextureVerteces(){
	char modelFileName[80] = "D:\\KIST\\models\\";
	std::string modelName = "doll";
	strcat(modelFileName, modelName.c_str());
	strcat(modelFileName, ".txt");
	FILE * file = fopen(modelFileName, "r");
	
    fscanf(file, "%d", &numVert);
    
	for (int i = 0; i < numVert; i++){
		fscanf(file, "%d", &modelPerVertex[i]);
    }
}

void readData(int mdl){

	std::string mdl_str = std::to_string(mdl);

	char fileName[80] = "D:\\KIST\\models\\";
	strcat(fileName, mdl_str.c_str());
	strcat(fileName, ".txt");

	FILE * fail = fopen(fileName, "r");
	fscanf(fail, "%f", &mu_k[mdl]);

	for (int i = 0; i < 2; i++)
		fscanf(fail, "%f", &featMin[mdl][i]);

	for(int i=0; i<2; i++)
		fscanf(fail, "%f", &featMax[mdl][i]);

	fscanf(fail, "%f", &velNormCoeffX[mdl]);
	fscanf(fail, "%f", &forceNormCoeff[mdl]);

	
	fscanf(fail, "%d", &rbfNum[mdl]);
	fscanf(fail, "%d", &nARCoeff[mdl]);


	if (rbfNum[mdl] * nARCoeff[mdl] <= MAX_NUM_RBF){
		for (int i = 0; i < rbfNum[mdl] * nARCoeff[mdl]; i++){
			fscanf(fail, "%lf", &rbfCoeff[mdl][i]);
		}

		for (int i = 0; i<rbfNum[mdl]; i++){
			fscanf(fail, "%f %f", &rbfCenters[mdl][i][0], &rbfCenters[mdl][i][1]);
		}

	} else {
		printf("Number of RBF exceeded maximum");
	}

	for (int i = 0; i < 3 * nARCoeff[mdl]; i++)
		fscanf(fail, "%lf", &polyCoeff[mdl][i]);

	// Make competible with old code
	isARMA = false;
	coeffNum[mdl] = nARCoeff[mdl] - 2;

}

