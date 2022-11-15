#include "../include/LZW.h"

LZW::LZW(int bits, std::vector<int> alphabet){
    this->bits = bits;
    this->alphabet = alphabet;
    this->nextIndex = 0;
    for(auto character : alphabet){
        this->dictionary[{character}] = this->nextIndex++;
    }
    this->dictionaryLength = pow(2, bits);
}


std::vector<int> LZW::compress(std::vector<u_char> file){
    std::vector<int> compressedFile;
    std::vector<int> characters;

    for (size_t i = 0; i < file.size(); i++){
        characters.push_back(file[i]);
        if(this->dictionary.find(characters) != dictionary.end()){
            continue;
        }

        if(this->nextIndex <= this->dictionaryLength)
            this->dictionary[characters] = this->nextIndex++;  
        characters.pop_back();
        compressedFile.push_back(dictionary[characters]);
        characters.clear();
        i--;
    }
    
    return compressedFile;
}

std::vector<int> LZW::decompress(std::vector<u_char> file){
    throw "Not implemented yet!";
}