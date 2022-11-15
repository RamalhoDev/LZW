#ifndef LZW_H
#define LZW_H
#include <vector>
#include <map>
#include <math.h>

class LZW{
    int bits;
    int nextIndex;
    int dictionaryLength;
    std::vector<int> alphabet;
    std::map<std::vector<int>, int> dictionary;

    
    public:
        LZW(int bits, std::vector<int> alphabet);
        std::vector<int> compress(std::vector<u_char> file);
        std::vector<int> decompress(std::vector<u_char> file);
};

#endif // !LZW_H