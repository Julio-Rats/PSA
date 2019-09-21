#ifndef  PSA
#define   PSA

#include <iostream>
#include <cstdlib>
#include <list>
#include <vector>
#include <fstream>
#include "./../Simulador CANbus/simulador.hpp"

#define     LENGTH_CHAR           64
#define     TIME_SIMULATION       12000

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
      unsigned int transcend;
      std::vector<Solution*> dominated;
};


/******************************************************************

Variaveis de controle do simulated anneling

*******************************************************************/

double    psa_starting_temperature;     // Temperatura inicial do PSA
double    psa_final_temperature;         // Temperatura final do PSA
double    psa_alpha_temperature;        // Escalar multiplicativo para decaimento da Temperatura
double    psa_alpha_scale;              // Escalar multiplicativo para função objetiva
double    psa_number_neighbor;           

Solution  psa_solution_neighbor;        // Solução Vizinho da atual
Solution  psa_solution_first;            // Solução atual do PSA

unsigned int        length_frames;      // Numero de frames na solução
unsigned int        length_cloud;       // Numero de soluções na nuvem de soluções
unsigned int        length_frontier;    // Numero de soluções na nuvem de soluções
std::vector<Solution> cloud_solutions;    // Conjunto com todas as soluções atuais
std::vector<Solution> cloud_frontier;     // Conjunto com todas as soluções "melhores"

Simulator_CAN* simulator;

char            psa_arq_result[LENGTH_CHAR];
/******************************************************************

Escopos das funções implementadas

*******************************************************************/

void     starting_cloud();
void     pareto_simulated_anneling();
void     random_scale(Solution& solucao);
void     run_simulation(Solution& solucao);
void     add_cloud_frontier(Solution new_s);
void     adjusts_scale(Solution& new_s, Solution base_s);
bool     is_dominat(Solution s1, Solution s2);
bool     accept(Solution s1, Solution s2, double temp);
std::vector<std::vector<Solution>> get_frontiers();
Solution random_neighbor(Solution base);
Solution neighborhood_search(Solution solucao);

#endif
