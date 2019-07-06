#ifndef SIMULADOR
#define  SIMULADOR

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "get_candb.h" // include to structs.h, fifo.h

/* defines para tipos booleanos*/
#define FALSE                 0
#define TRUE                  1

/* Variavel de controle para debug passo a passo do simulador CANbus*/
#define DEBUG                 FALSE
/* Variavel de controle para habilitar saida no terminal das frames lidas do arquivo*/
#define PRINT_FRAMES          FALSE
/* Variavel de controle para saida de analises no terminal*/
#define RESULTS               FALSE

/* Arquivo de saida para Log de Bests gerado no S.A */
extern FILE*  Arq_Log_Best;

/* Variavel de controle para escrita do arquivo de log BEST solution*/
extern bool logframes;

extern double     wcrt;
extern double     end_time_queue;
extern double     time_min_queue;
extern double     time_max_queue;
extern double     time_mean_queue;
extern double     start_time_queue;
extern double     busload_simulated;
extern double     time_current_simulation;

extern u_int32_t  frames_write;
extern u_int16_t  length_queue;
extern u_int16_t  msg_deadline;
extern u_int16_t  number_of_queue;
extern u_int16_t  max_length_queue;
extern u_int16_t  min_length_queue;
extern u_int16_t  acumul_length_queue;
extern u_int16_t  current_length_queue;

fifo_t* get_priority_frame();
double small_time(double time);
double get_mean_wcrt();
void   get_wcrt();
void   check_time();
void   verific_wcrt();
void   verific_queue();
void   free_recurses();
void   verific_deadlines();
void   gravaLogFramesCab();
void   realloc_event(fifo_t* event);
void   gravaLogFrames(event_t event);
void   add_time_lost_arbitrage(double time);
void   start_simulation(double time_of_simulation);
int    main_simulated(char* path, double time_simulation);

#endif
