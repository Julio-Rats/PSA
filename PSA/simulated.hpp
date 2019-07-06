#ifndef  PSA
#define   PSA

#include <iostream>
#include <cstdlib>
#include <list>
#include "./../Simulador deterministico/simulador.h"

#define       LENGTH_CHAR           64
#define       TIME_SIMULATION       120000

class Solution;
class Frame_CAN;

/******************************************************************

Estrutura (classe) da solução do problema

*******************************************************************/

class Solution
{
  public:
      double       wcrt;                  //  Pior tempo de atraso cumulado
      double       scale_wcrt;            //  Escalar para função Objetiva
      double       frames_burst;          //  Numero maximo de frames ocorrido em  um Burst
      double       scale_frames_burst;    //  Escalar para função Objetiva
      double       time_mean_burst;       //  Tempo medio dos bursts
      double       scale_time_mean_burst; //  Escalar para função Objetiva
      Frame_CAN*   candb_solution;        //  Conjunto de frames

};

/******************************************************************

Estrutura (classe) para representar um frame CAN

*******************************************************************/

class Frame_CAN
{
  public:
      unsigned int id:11;               // ID da mensagem
      double       cycle_time;          // Periodo de ciclo
      double       deadline_time;       // Tempo de limite maximo em fila
      double       delay_time;          // Tempo proposto para start delay (offset)
};

/******************************************************************

Variaveis de controle do simulated anneling

*******************************************************************/

double    psa_starting_temperature;     // Temperatura inicial do PSA
double    psa_final_temperature;         // Temperatura final do PSA
double    psa_alpha_temperature;        // Escalar multiplicativo para decaimento da Temperatura

Solution  psa_solution_neighbor;        // Solução Vizinho da atual
Solution  psa_solution_current;         // Solução atual do PSA

unsigned int        length_frames;      // Numero de frames na solução
unsigned int        length_cloud;       // Numero de soluções na nuvem de soluções
std::list<Solution> cloud_solutions;    // Conjunto com todas as soluções atuais

char      psa_arq_times_simulation[LENGTH_CHAR] = ".tempos.txt";
char      psa_arq_result[LENGTH_CHAR];
/******************************************************************

Escopos das funções implementadas

*******************************************************************/

void     pareto_simulated_anneling();
void     starting_cloud();
void     run_simulation(Solution& solucao);
Solution neighborhood_search(Solution solucao);
Solution random_neighbor(Solution base);

#endif
