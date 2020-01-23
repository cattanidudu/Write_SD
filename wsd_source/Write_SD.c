/*
 * Write_SD.c
 *
 *      Autor: Cattani, E.
 *      Orientador: Padilha, R.
 *
 *      GEPOC - Grupo de Eletrônica de Potencia e Controle
 *
 *      Codigo para realizar escritas em tempo real utilizando um cartão SD
 *      Compatível com a familia F2837xD_
 *
 *      Versao: 0.1
 */


#include "F28x_Project.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"

// --------------------------------- DEFINICOES --------------------------------- //

#define f_writing 10                        // em KHz - frequencia de escrita
#define f_control 10                        // em KHz - frequencia de controle
#define n_variables 2                       // numero de variaveis

#define cluster_size n_variables*50         // ser multiplo de 125
#define buffer_size_sd cluster_size*10      // tamanho total do buffer
                                            // aumentar a fator que multiplica
                                            // para aumentar o tempo livre da CPU

#define cycles_sync_fatfs f_control*1000/100
#define cycles_sync_write (f_control/f_writing)

// ------------------------- INICIALIZACAO DAS VARIAVEIS ------------------------- //

// FATFS
FRESULT fresult_mount = FR_NO_FILE;
FRESULT fresult_open = FR_NO_FILE;
FRESULT fresult_write = FR_NO_FILE;
FRESULT fresult_save = FR_NO_FILE;
FATFS g_sFatFs;
FIL g_sFileObject;

// BUFFERS

float buffer_sd[buffer_size_sd]; // buffer circular
WORD data_byte[cluster_size*4];

// CONTADORES

unsigned int counter_diff = 0;
unsigned int bytes_written = 0;
unsigned int cycle_number_w = 0;
unsigned int cycle_number_f = 0;
unsigned int i_pin;
unsigned int counter_data_byte;

// PONTEIROS

float *p_in =  buffer_sd; // comeca pelo buffer zero
float *p_out =  buffer_sd;
WORD *p_data_byte = data_byte;

// ----------------------------------- METODOS ----------------------------------- //

void init_link()
// Deve ser chamado antes da inicialização do ePWM, para que não ocorram conflitos
// Ocorre a configuração do clock de baixa velocidade para que o CLK do SPI seja 12.5MHz
{
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 2;
    EDIS;
    fresult_mount = f_mount(0, &g_sFatFs);
    int path_counter = 0;
    char path[20];
    do{

        sprintf(path, "data%d.txt", path_counter);
        path_counter++;
        fresult_open = f_open(&g_sFileObject, path, FA_WRITE | FA_CREATE_NEW);
    } while(fresult_open == FR_EXIST );

    //FA_CREATE_ALWAYS

}

void check()
// Precisa ser chamada continuamente na main()
// Essa função realiza a conversao do byte 16-bit para o byte 8-bit
// Realiza o manejo dos buffers e chama os métodos f_write e f_sync do fatfs
{
    if ((counter_diff * buffer_size_sd + p_in - cluster_size)>=(p_out))
        {
            for (counter_data_byte=0;counter_data_byte<cluster_size;counter_data_byte++)
            {
                *p_data_byte = __byte(p_out,(counter_data_byte)*4+3);
                p_data_byte++;
                *p_data_byte = __byte(p_out,(counter_data_byte)*4+2);
                p_data_byte++;
                *p_data_byte = __byte(p_out,(counter_data_byte)*4+1);
                p_data_byte++;
                *p_data_byte = __byte(p_out,(counter_data_byte)*4);
                p_data_byte++;
            }
            p_data_byte = data_byte;
            fresult_write = f_write(&g_sFileObject,data_byte,sizeof(data_byte), &bytes_written);
            p_out = p_out+cluster_size;
            if (p_out == buffer_sd + buffer_size_sd)
                {
                p_out = buffer_sd;
                counter_diff--;
                fresult_save = f_sync(&g_sFileObject);
                GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;
                }
        }
}

void write(float data_rec[])
    {
    // Essa função precisa ser chamada em todos os ciclos de controle, mesmo que não vá escrever nada
    // O argumento dessas é um vetor com os dados a serem escritos

    cycle_number_w++;
    cycle_number_f++;

    if (cycle_number_f == cycles_sync_fatfs)
    {
        cycle_number_f = 0;
        disk_timerproc();
    }

    if (cycle_number_w == cycles_sync_write)
    {
        cycle_number_w = 0;

        for (i_pin=0;i_pin<n_variables;i_pin++) // colocar os dados do vetor recebido no buffer
        {
            *p_in = data_rec[i_pin];
            p_in++;
        }

        if( p_in == (buffer_sd + buffer_size_sd) ) // reinicia o buffer
        {
           p_in = buffer_sd;
           counter_diff++;
           GpioDataRegs.GPATOGGLE.bit.GPIO19 = 1;
        }
    }
    }

void test()
    // pode ser chamado na main() ao invés da write(), para executar um teste da continuidade dos dados
    // ser chamado em todas interrupções de controle, caso utilizado
    {
    static float data[n_variables];
    static float *p_data = data;
    static float counter_test=0;
    static int counter_data;

    int step = 0.5;

    for(counter_data=0;counter_data<n_variables;counter_data++)
    {
    *p_data = counter_test + counter_data;
    p_data ++;
    }

    p_data = data;
    counter_test+=step;
    if (counter_test == 15000) step *= -1;
    if (counter_test == 0) step *= -1;

    write(data);

    }





