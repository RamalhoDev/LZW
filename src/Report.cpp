#include "../include/Report.h"

Report::Report(string instanceName, int fileSize, int compressedFileSize, int dictionaryLength, int indexBitLength, double seconds){
    this->fileSize = fileSize;
    this->compressedFileSize = compressedFileSize;
    this->dictionaryLength = dictionaryLength;
    this->indexBitLength = indexBitLength;
    this->seconds = seconds;
    this->instanceName = instanceName.substr(0, instanceName.rfind(".")) + ".json";
}


void Report::writeReport(){
    ofstream file(this->instanceName, ios::out);

    string instance = this->instanceName.substr(instanceName.rfind("/")+1, instanceName.rfind("_") - instanceName.rfind("/") - 1);
    int dictionaryMaxSize = pow(2,this->indexBitLength);
    file << "{ \"instance\": \"" + instance + "\",";
    file << "\"file_size\":" << this->fileSize << ",";
    file << "\"compressed_file_size\":" << this->compressedFileSize << ",";
    file << "\"dictionary_length\":" << this->dictionaryLength << ",";
    file << "\"bits_size\":" << this->indexBitLength << ",";
    file << "\"dictionary_max_length\":" << dictionaryMaxSize << ",";
    file << "\"compression_ratio\":" << (double) this->fileSize/this->compressedFileSize << ",";
    file << "\"compression_ratio2\":" << (double) this->fileSize / (this->dictionaryLength * this->indexBitLength / 8.0) << ",";
    file << "\"execution_time\":" << seconds << ",";
    file << "\"used_all_indexes\":" << ((dictionaryMaxSize == dictionaryLength)?"true":"false") << "}";
    file.close();
}