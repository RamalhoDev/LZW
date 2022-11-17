#ifndef LZW_H
#define LZW_H
#include <vector>
#include <map>
#include <math.h>
#include <iostream>

class LZW{
    int bits;
    int nextIndex;
    int dictionaryLength;
    std::vector<int> alphabet;
    std::map<std::vector<int>, int> dictionary;
    std::vector<std::vector<int>> indexDictionary; 

    
    public:
        LZW(int bits, std::vector<int> alphabet);
        std::vector<int> compress(std::vector<u_char> file);
        std::vector<int> decompress(std::vector<int> file);
};

#endif // !LZW_H