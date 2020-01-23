/*
 * Write_SD.h
 *
 *      Autor: Cattani, E.
 *      Orientador: Padilha, R.
 *
 *      GEPOC - Grupo de Eletr�nica de Potencia e Controle
 *
 *      Codigo para realizar escritas em tempo real utilizando um cart�o SD
 *      Compat�vel com a familia F2837xD_
 *
 *      Versao: 0.1
 */


#ifndef WRITE_SD_H_
#define WRITE_SD_H_
extern void init_link();
extern void write(float data[]);
extern void check();
extern void test();
#endif /* WRITE_SD_H_ */
