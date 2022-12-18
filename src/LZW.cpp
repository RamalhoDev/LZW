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
void LZW::writeDictionary(std::string filePath){
  std::ofstream file(filePath, std::ios::out | std::ios::binary);

  for(auto dict_index_pair : this->dictionary){
    file.write(reinterpret_cast<const char *> (&dict_index_pair.second),sizeof(dict_index_pair.second));
    auto dict_size = dict_index_pair.first.size();
    file.write(reinterpret_cast<const char *> (&dict_size), sizeof(dict_size));
    file.write((char*)&dict_index_pair.first[0], dict_index_pair.first.size() * sizeof(int));
  }
  file.close();
  return;
}

void LZW::readDictionary(std::string filePath){
    this->staticDict = true;
    std::ifstream input(filePath, std::ios::binary );
    
    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    input.seekg(0, std::ios::beg);

    int index = -1;
    int sizeVector = -1;
    this->indexDictionary = std::vector<std::vector<int>>(this->dictionaryLength);
    while(input.tellg() < size){

        input.read(reinterpret_cast<char*>(&index), sizeof(int));
        input.read(reinterpret_cast<char*>(&sizeVector), sizeof(size_t));
        std::vector<int> characters(sizeVector);
        input.read(reinterpret_cast<char*>(&characters[0]), sizeof(int) * sizeVector);

        this->dictionary[characters] = index;
        this->indexDictionary[index] = characters;
    }
    this->nextIndex = this->dictionaryLength;
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