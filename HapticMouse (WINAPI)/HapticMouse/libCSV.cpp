#include "libCSV.h"

std::vector<std::string> libCSV::ParseRow(const std::string& row) {
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
        stringArray = ParseRow(row);
    }
    return stringArray;
}

std::ifstream libCSV::OpenFile(std::string filename)
{
    std::ifstream file(filename);
    return file;
}

std::ofstream libCSV::OpenWrFile(std::string filename)
{
    std::ofstream file;
    file.open(filename, std::ios_base::app);
    return file;
}

void libCSV::CloseFile(std::ifstream& in)
{
    in.close();
}

void libCSV::CloseFile(std::ofstream& out)
{
    out.close();
}

libCSV::libCSV() {}

libCSV::libCSV(std::string fileName)
{
    this->fileName = fileName;
}

std::vector<double> libCSV::readVals()
{
    std::vector<double> vals;
    std::ifstream stream = this->OpenFile(this->fileName);
    std::vector<std::string> stringVals = this->readLineCSV(stream);
    this->CloseFile(stream);
    for (std::string element : stringVals)
    {
        double temp = ::atof(element.c_str());
        vals.push_back(temp);
    }
    return vals;
}

bool libCSV::WriteVals(std::vector<double> vals)
{
    bool result = true;
    std::ofstream stream = this->OpenWrFile(this->fileName);
    
    for (double value : vals)
    {
        this->AppendValue(stream, value);
    }
    this->CloseFile(stream);
    return result;
}

std::ofstream libCSV::BeginWrite()
{
    std::ofstream stream = this->OpenWrFile(this->fileName);
    return stream;
}

void libCSV::EndWrite(std::ofstream& stream)
{
    this->CloseFile(stream);
}

void libCSV::AppendValue(std::ofstream& stream, double value)
{
    char buffer[200];
    sprintf(buffer, "%F,", value);
    stream << buffer;
}