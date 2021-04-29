#include "libCSV.h"

std::vector<std::string> libCSV::parseRow(const std::string& row) {
    std::vector<std::string> fields;
    std::string temporary = "";
    size_t i = 0; // index of the current field
    for (char c : row) {
        switch (c)
        {
        case ',': // end of field
            fields.push_back(temporary);
            temporary = "";
            i++;
            break;
        default:  temporary.push_back(c);
            break;
        }
    }
    return fields;
}

std::vector<std::string> libCSV::readLineCSV(std::istream& in) {
    std::vector<std::string> stringArray;
    std::string row;
    while (!in.eof()) {
        std::getline(in, row);
        if (in.bad() || in.fail()) {
            break;
        }
        stringArray = parseRow(row);
    }
    return stringArray;
}

std::ifstream libCSV::openFile(std::string filename)
{
    std::ifstream file(filename);
    return file;
}

libCSV::libCSV() {}

libCSV::libCSV(std::string fileName)
{
    this->fileName = fileName;
}

std::vector<double> libCSV::readVals()
{
    std::vector<double> vals;
    std::ifstream stream = this->openFile(this->fileName);
    std::vector<std::string> stringVals = this->readLineCSV(stream);
    for (std::string element : stringVals)
    {
        double temp = ::atof(element.c_str());
        vals.push_back(temp);
    }
    return vals;
}