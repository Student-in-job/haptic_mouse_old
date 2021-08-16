#pragma once
//#include <istream>
#include <fstream>
#include <string>
#include <vector>

class libCSV
{
private:
	std::string fileName;
	std::vector<std::string> ParseRow(const std::string& row);
	std::ifstream OpenFile(const std::string filename);
	std::ofstream OpenWrFile(const std::string filename);
	void CloseFile(std::ifstream& in);
	void CloseFile(std::ofstream& out);
	std::vector<std::string> readLineCSV(std::istream& in);
public:
	libCSV();
	libCSV(std::string filename);
	std::vector<double> readVals();
	bool WriteVals(std::vector<double> vals);
	std::ofstream BeginWrite();
	void EndWrite(std::ofstream& stream);
	void AppendValue(std::ofstream& stream, double value);
};

