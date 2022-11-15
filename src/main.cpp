#include <iostream>
#include <fstream>
#include "../include/LZW.h"

using namespace std;

#define SIZE 130000000
unsigned char *buffer = new unsigned char[SIZE];
int pos = 0; // posicao em bits
FILE *output;
const char *name = "output.lzw";

void lerArquivo(const char *name) {
    unsigned char byte;
    FILE *input = fopen(name, "rb");
    if (input <= 0) {
        printf("Erro abrindo o arquivo %s\n", name);
        return;
    }
    
    while (!feof(input)) {
        fread(&byte, 1, 1, input);
        if (feof(input))
            break;
        printf("0x%02X ", byte);
    }
    printf("\n");
    fclose(input);
}

void addBits(int value, int qBits)
{
    int byte = pos / 8; // posicao do proximo byte com bits disponiveis
    int shift = pos % 8; // quant de bits utilizados
    int disp = 8 - shift; // quant de bits disponiveis
    unsigned int mask = 0;
    int falta = qBits - disp; // bits que faltam para o proximo byte
    int falta2 = 0; // bits que faltan para o segundo proximo byte
    int i;
    if (falta > 8) {
        falta2 = falta - 8;
        falta = 8;
    }
    
    //printf("value=%3d::byte=%d, shift=%d, disp=%d, mask=%d, falta=%d, falta2=%d\n", value, byte, shift, disp, mask, falta, falta2);
    
    mask = 0;
    for (i = 0 ; i < disp ; i++)
        mask = mask | (1 << i);
    buffer[byte] = (buffer[byte] & (~mask & 0xFF)) | ((value >> (falta + falta2)) & mask);
    //printf("mask1=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));

    if (falta > 0) {
        mask = 0;
        for (i = 0 ; i < (8 - falta) ; i++)
            mask = mask | (1 << i);
        if (falta2 == 0)
            buffer[byte + 1] = (buffer[byte + 1] & mask) | ( (value << (8 - falta)) & (~mask & 0xFF));
        else
            buffer[byte + 1] = ((value >> falta2) & 0xFF);
        //printf("mask2=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    if (falta2 > 0) {
        mask = 0;
        for (i = 0 ; i < (8 - falta2) ; i++)
            mask = mask | (1 << i);
        buffer[byte + 2] = (buffer[byte + 2] & mask) | ((value << (8 - falta2)) & (~mask & 0xFF)); 
        //printf("mask3=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    pos += qBits;

    // Adicionado para salvar os bytes as poucos.
    byte = pos / 8; // posicao do proximo byte com bits disponiveis
    if (byte >= SIZE - 3) {
        fwrite(buffer, 1, byte, output);
        //printf("salvei %d bytes\n", byte);
        pos -= byte * 8;
        shift = pos % 8; // quant de bits utilizados
        for (i = 0 ; i < (8-shift) ; i++)
            mask = mask | (1 << i);
        buffer[0] = buffer[byte] & ~mask;
    }
}

void salvaFim() {
    int byte = pos / 8; // posicao do proximo byte com bits disponiveis
    int shift = pos % 8;
    if (shift > 0)
        byte++;
    if (byte > 0) {
        fwrite(buffer, 1, byte, output);
        //printf("salvei fim %d bytes\n", byte);
    }
    fclose(output);
}


int main(int argc, char * argv[]){
    if(argc < 2)
        throw "Missing K argument!";
    else if(argc < 3)
        throw "Missing file path!";
 
    int K = atoi(argv[1]);
    string filePath = string(argv[2]);

    vector<int> alphabet;
    for (size_t i = 0; i < 256; i++)    {
        alphabet.push_back(i);
    }

    cout << "K = " << K << endl << "Alphabet = ";
    for(auto character : alphabet){
         cout << character << " ";
    }
    cout << endl;

    ifstream file(filePath, ios::binary);
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<u_char> fileData(fileSize); 
    file.read((char*) &fileData[0], fileSize);

    LZW lzw = LZW(K, alphabet);
    auto compressedFile = lzw.compress(fileData);
    output = fopen(name, "wb");
    if (output <= 0) {
        printf("Erro abrindo o arquivo %s\n", name);
        return -1;
    }
    
    for(auto index: compressedFile){
        addBits(index, K);
    }
    salvaFim();
    lerArquivo(name);

    return 0;
}