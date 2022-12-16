#include "../include/LZW.h"

LZW::LZW(int bits, std::vector<int> alphabet){
    this->bits = bits;
    this->alphabet = alphabet;
    this->nextIndex = 0;
    for(auto character : alphabet){
        this->dictionary[{character}] = this->nextIndex++;
        this->indexDictionary.push_back({character});
    }
    this->dictionaryLength = pow(2, bits);
}


std::vector<int> LZW::compress(std::vector<u_char> file){
    std::vector<int> compressedFile;
    std::vector<int> characters;

    for (size_t i = 0; i < file.size(); i++){
        characters.push_back(file[i]);
        if(this->dictionary.find(characters) != dictionary.end())
            continue;

        if(this->nextIndex < this->dictionaryLength){
            this->dictionary[characters] = this->nextIndex++;  
            this->indexDictionary.push_back(characters);  
        }

        characters.pop_back();
        compressedFile.push_back(dictionary[characters]);
        characters.clear();
        i--;
    }
    if(characters.size())
        compressedFile.push_back(dictionary[characters]);
    
    return compressedFile;
}

void LZW::write_dictionary(std::string file_path){
  return;
}

std::vector<int> LZW::decompress(std::vector<int> file){
    std::vector<int> decompressedFile;
    std::vector<int> previous;

    for (int entry : file){
        std::vector<int> decodedMessage;
        if(indexDictionary.size() <= entry){
            decodedMessage = previous;
            decodedMessage.push_back(previous[0]);
        }else{
            decodedMessage = indexDictionary[entry];
        }

        decompressedFile.insert(decompressedFile.end(),decodedMessage.begin(), decodedMessage.end());

        if(previous.size()){
            std::vector<int> newEntry = previous;
            newEntry.push_back(decodedMessage[0]);

            if(this->nextIndex <= this->dictionaryLength && dictionary.find(newEntry) == dictionary.end()){
                this->dictionary[newEntry] = this->nextIndex++;
                this->indexDictionary.push_back(newEntry);  
            }
        }

        previous = indexDictionary[entry]; 
    }    

    return decompressedFile;
}