# LZW

C++ LZW compression library.

Para criação do makefile use os seguintes comandos:
```
mkdir build
cd build
cmake ..
```
Para compilação do programa execute o seguinte comando dentro da pasta **build**:
```
make
```
O arquivo possui 4 argumentos disponíveis:

1. Caminho para o arquivo de input;
2. Operação que será realizada ("compress" ou "decompress").
3. K bits utilizados para representação índice do dicionário e limitação do tamanho do dicionário. (opcional) Valor padrão: 9;
4. Caminho para o arquivo de saída (opcional)

Comando dentro do build:

```
./iti_lzw caminho_para_arquivo_de_entrada operação número_de_bits caminho_para_arquivo_de_saída caminho_para_escrever_dicionario
```
