#pragma once
#include <istream>
#include <fstream>
#include <string>
#include <vector>

class libCSV
{
private:
	std::string fileName;
	std::vector<std::string> parseRow(const std::string& row);
	std::ifstream openFile(const std::string filename);
	std::vector<std::string> readLineCSV(std::istream& in);
public:
	libCSV();
	libCSV(std::string filename);
	std::vector<double> readVals();
};

