#include <iostream>
#include <fstream>
#include "../include/LZW.h"

using namespace std;
#define OPERATION "compress"
#define OUTPUT_PATH "output"
#define SIZE 4096

unsigned char *buffer = new unsigned char[SIZE];
int pos = 0; // posicao em bits
FILE *output;

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

int getBits(int qBits)
{
    int value = 0;
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
    value = (buffer[byte] & mask) << (falta + falta2);
    //printf("mask1=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));

    if (falta > 0) {
        if (falta2 == 0)
            value = value | ((buffer[byte + 1]) >> (8 - falta)); 
        else
            value = value | (buffer[byte + 1] << falta2); 
        //printf("mask2=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    if (falta2 > 0) {
        //mask = 0;
        //for (i = 0 ; i < (8 - falta2) ; i++)
        //    mask = mask | (1 << i);
        value = value | (buffer[byte + 2] >> (8 - falta2)); 
        //printf("mask3=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    pos += qBits;
    
    return value;
}

int main(int argc, char * argv[]){
    if(argc < 2)
        throw "Missing K argument!";
    else if(argc < 3)
        throw "Missing file path!";
        
    // Read command line arguments
    // First argument is the length of bits to use for index encoding
    // Second argument is the file to be compressed or decompressed
    // Third argument is the operation to execute (compress or decompress). Default compress.
    // Forth argument is the output path file.

    int K = atoi(argv[1]);
    string filePath = string(argv[2]);
    string operation = OPERATION;
    string outputPath = OUTPUT_PATH;
    
    if(argc >= 4)
        operation = string(argv[3]);
    if(argc >= 5)
        outputPath = string(argv[4]);

    // Create alphabet.
    vector<int> alphabet;
    for (size_t i = 0; i < 256; i++)    {
        alphabet.push_back(i);
    }

    // Open input file.
    ifstream file(filePath, ios::binary);
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<u_char> fileData(fileSize); 
    file.read((char*) &fileData[0], fileSize);


    LZW lzw = LZW(K, alphabet);
    if(operation == "compress"){
        //Compress input file
        cout << "Compression started!\n";
        auto compressedFile = lzw.compress(fileData);
        cout << "Compression finished!\n";

        // Write to output file using k bits for each index
        output = fopen(outputPath.c_str(), "wb");
        if (output <= 0) {
            printf("Erro abrindo o arquivo %s\n", outputPath.c_str());
            return -1;
        }

        for(auto index: compressedFile){
            addBits(index, K);
        }
        salvaFim();
    }else{
        //Read compressed file data
        buffer = reinterpret_cast<u_char *>(fileData.data());
        
        int indexCount = (fileSize * 8/K)+1;
        vector<int> data(indexCount);
        for (size_t i = 0; i < indexCount; i++)
            data[i] = getBits(K);
        
        //Execute input file decompression
        cout << "Decompression started\n";
        auto decompressedFile = lzw.decompress(data);
        cout << "Decompression finished\n";

        // Write decompressed input file
        vector<char> newFile;
        for(int c: decompressedFile)
            newFile.push_back((char) c);

        ofstream outputFile(outputPath, ios::binary | ios::out);
        
        outputFile.write((char *) &newFile[0], newFile.size());
        outputFile.close(); 
    }
    
    return 0;
}