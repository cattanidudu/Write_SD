
# WriteSD

  - Algortimos desenvolvidos para realizar escritas em tempo real utilizando um cartão SD
  - Compatível com a familia F2837xD_

## Conteúdo

  - Possuí os códigos fornecidos pela *Texas Instruments* para interfaceamento do DSP com SPI e o FatFs para realizar as escritas em um arquivo FAT.
  - Possuí os códigos desenvolvidos que realizam o manejo dos dados de tal forma que seja possível a escrita em tempo real das variáveis.
  - Possuí o código para realizar a decodificação e validação dos floats escritos (em .m).

## Instalação

### Code Composer Studio

  - É necessário incluir os *headers* no diretório do projeto. Para isso, clicar em Propriedades>CCS Build>C2000 Compiler>Include Options e adicionar o diretório /wsd_headers.
  - Também é necessário adicionar as fontes dos códigos na raiz do projeto, para isso, clicar em AddFiles...> e selecionar os arquivos: 
```
ff.c 
mmc_F2837x.c
Write_SD.c
```
### Matlab

  - Incluir diretório do algoritmo em Matlab utilizado pelo usuário que será utilizado os arquivos:
  ```
  decoder_Sd.m
  test_Sd.m
  ```
  
## Hardware
  - Devem ser feitas tais conexões elétricas:
  
  GPIO | Pino   | Função
  ---- | ------ | -------
  58   | #15    | MOSI
  59   | #14    | MISO
  60   | #7     | SCK
  61   | #19    | CS

## Métodos de cada sistema

  - Foram desenvolvidos diversos métodos para a gravação e validação dos dados escritos.
  
  ### Escrita em operação normal
  - Incluir no cabeçalho do código no CCS o código de escrita no SD
  ```
  #include "Write_SD.h"
  ```
  - No corpo inicial do código no CCS, antes da inicialização do EPWM, para realizar todos os procedimentos iniciais, chamar o método:
  ```
  init_link()
  ```
  - Em todas interrupções do sistema de controle, deve-se montar vetor do tipo *float* com os dados a serem escritos e chamar o método para tal operação:
  ```
  float data[n_variables];
  ...
  // preencher os devidos valores no vetor *data*
  write(data); 
  
  Exemplo:
  float data[5];
  ...
  data = {1,2,3,4};
  write(data);
  ```
  - Após realizada todo o processo, deve-se utilizar a função *decode_Sd()* no Matlab:
  ```
  data = decode_Sd(path,[Variables_names])
  
  Exemplo:
  data = decode_Sd("E:/data.txt",["Corrente","Tensão"]);
  // Deve-se atentar que a ordem do nome das variáveis deve seguir 
  o mesmo padrão que foi escrito no método *write()* no CCS.
  ```
### Escrita em operação de teste
  O processo é extremamente similar, deve-se:
  - Incluir o cabeçalho *#include "Write_SD.h"*;
  - Incluir o método *init_link()* no corpo de inicialização;
  - Incluir na interrupção do controle o método test() ao invés do método *write(...)*, simplesmente desse modo:
    ```
    test()
    // deve-se atentar que esse método não recebe nenhum argumento
    ```
  - No Matlab o método e argumetos são diferentes:
    ```
    test_Sd(path, n_variables)
    
    Exemplo:
    test_Sd("E:/data.txt",4)
    ```
## Configurações
No arquivo "write_Sd.c" encontram-se algumas definições a serem realizadas;
- #define f_writing **x**-- Utilizado para definir a frequência em que os dados serão escritos (em KHz)
- #define f_control **x**-- Utilizado para definir a frequência de controle (em KHz)
- #define cluster_size n_variables* **x** -- Deve-se definir x de tal forma que o tamanho do cluster seja múltiplo de 125
- #define buffer_size_sd cluster_size* **x** -- Deve-se definir o menor x possível sem que comprometa a escrita dos dados
Quanto menor o buffer, menos memória utilizada, porém mais chance de ocorrer erro. Para aumentar o desempenho, aumentar o buffer. Ver no setor de desempenho abaixo.

### Possíveis configurações
  
  Variaveis | f_writing (KHz)  | f_control (KHz) |x - buffer_size_sd  | x - cluster_size | Tempo para escrita/ciclo |
  :----:    | :------:         | :-------:       | :-----:            | :-----:          | :----:                   |
  2         | 10               | 10              |  50                | 62               | 60uS                     |
  5         | 5                | 10              |  15                | 100              | 60uS                     | 

## Observações

- Esse algoritmo esta configurado para utilização com a SPI-A, porém no arquivo *mmc_F2837x.c* tais configurações podem ser modificadas.
- Sempre o nome do arquivo é "data.txt". Caso já tenha algum outro arquivo do mesmo nome no diretório, o algortimo sobreescreve.
- Pode-se ter certeza que o sistema esta funcionando devidamente se for FR_OK o valor das variáveis:
```
fresult_mount
fresult_open
fresult_write
fresult_save
```
# Desenvolvido por

 - Autor: Eduardo Cattani
 - Orientador: Rodrigo Padilha
 - Grupo: GEPOC - Grupo de Eletrônica de Potencia e Controle
 - Contato: cattani98@gmail.com
