#ifndef REPORT_H
#define REPORT_H

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
using namespace std;

class Report{
    int fileSize;
    int compressedFileSize;
    int dictionaryLength;
    int indexBitLength;
    string instanceName;
    double seconds;

    public:
        Report(string instanceName, int fileSize, int compressedFileSize, int dictionaryLength, int indexBitLength, double seconds);
        void writeReport();
};

#endif // !REPORT_H