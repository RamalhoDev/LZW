#include <iostream>
#include <chrono>
#include <fstream>
#include "../include/LZW.h"
#include "../include/Report.h"
#include <argparse/argparse.hpp>


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
    // fclose(output);
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
    argparse::ArgumentParser program("iti_lzw");
    program.add_argument("-i", "--input").required().help("Path to input file.");    
    program.add_argument("operation").help("Mode: 'compress' or 'decompress'.").default_value(std::string("compress"));    
    program.add_argument("-o", "--output").help("Output path.").default_value(std::string("a.out"));
    program.add_argument("dict_path").default_value(std::string("a.dict")).help("Read a dict file.");
    program.add_argument("number_of_bits").default_value(9).help("Number of bits.").scan<'i', int>();    

    try {  
        program.parse_args(argc, argv);
    }catch(const std::runtime_error& err){
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
  
    // Read command line arguments
    // First argument is the length of bits to use for index encoding
    // Second argument is the file to be compressed or decompressed
    // Third argument is the operation to execute (compress or decompress). Default compress.
    // Forth argument is the output path file.

    string filePath = program.get<std::string>("--input");
    string operation = program.get<std::string>("operation");
    string outputPath = program.get<std::string>("--output");
    string dictPath = program.get<std::string>("dict_path");
    int K = program.get<int>("number_of_bits");

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

        auto start = chrono::high_resolution_clock::now();

        auto compressedFile = lzw.compress(fileData);
        auto end = chrono::high_resolution_clock::now();
        double seconds = chrono::duration_cast<chrono::seconds>(end - start).count();
        cout << "Compression finished!\n";

        // Write to output file using k bits for each index
        outputPath += ".lzw";
        output = fopen(outputPath.c_str(), "wb");
        if (output <= 0) {
            printf("Erro abrindo o arquivo %s\n", outputPath.c_str());
            return -1;
        }

        for(auto index: compressedFile){
            addBits(index, K);
        }
        salvaFim();
        fseek(output, 0, SEEK_END);
        int size = ftell(output);
        fclose(output);

        if(dictPath != ""){
          lzw.write_dictionary(dictPath);
        }

        Report report = Report(outputPath, fileSize, size, lzw.getIndexesLength(), K, seconds);
        report.writeReport();
    }else{
        //Read compressed file data
        buffer = reinterpret_cast<u_char *>(fileData.data());
        
        int indexCount = (fileSize * 8/K);
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