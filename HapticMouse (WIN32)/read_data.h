#pragma once
#include "shared.h"
#include <fstream>
#include <string.h>

void readData(std::string folderPath, int mdl);
void loadModels(std::string folderPath, int mdls_count);

void loadModels(std::string folderPath, int mdls_count) {

    bool mdl_list[200];

    for (int i = 0; i < 200; i++)
        mdl_list[i] = false;

    // Including the models
    for (int i = 1; i <= mdls_count; i++) {
        mdl_list[i] = true;
    }

    //\ Including the models
    for (int i = 0; i <= mdls_count; i++) {
        if (!mdl_list[i]) continue;
        else {
            readData(folderPath, i);
        }
    }
}


void readData(std::string folderPath, int mdl) {

    std::string mdl_str = std::to_string(mdl);

    char* fileName = new char[folderPath.length() + 1];
    strcpy(fileName, folderPath.c_str());

    strcat(fileName, mdl_str.c_str());
    strcat(fileName, ".txt");

    FILE* fail = fopen(fileName, "r");


    fscanf(fail, "%f", &gainConst[mdl]);
    fscanf(fail, "%f", &mu_k[mdl]);

    for (int i = 0; i < 2; i++)
        fscanf(fail, "%f", &featMin[mdl][i]);

    for (int i = 0; i < 2; i++)
        fscanf(fail, "%f", &featMax[mdl][i]);

    fscanf(fail, "%f", &velNormCoeffX[mdl]);
    fscanf(fail, "%f", &forceNormCoeff[mdl]);


    fscanf(fail, "%d", &rbfNum[mdl]);
    fscanf(fail, "%d", &nARCoeff[mdl]);


    if (rbfNum[mdl] * nARCoeff[mdl] <= MAX_NUM_RBF) {
        for (int i = 0; i < rbfNum[mdl] * nARCoeff[mdl]; i++) {
            fscanf(fail, "%lf", &rbfCoeff[mdl][i]);
        }

        for (int i = 0; i < rbfNum[mdl]; i++) {
            fscanf(fail, "%f %f", &rbfCenters[mdl][i][0], &rbfCenters[mdl][i][1]);
        }

    }
    else {
        printf("Number of RBF exceeded maximum");
    }

    for (int i = 0; i < 3 * nARCoeff[mdl]; i++)
        fscanf(fail, "%lf", &polyCoeff[mdl][i]);

    // Make competible with old code
    isARMA = false;
    coeffNum[200] = nARCoeff[mdl] - 2;

}