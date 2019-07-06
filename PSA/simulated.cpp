#include "simulated.hpp"

using namespace std;

void pareto_simulated_anneling()
{
    /* Temperatura atual do PSA */
    double psa_current_temperature = psa_starting_temperature;

    /* Preenche nuvem com "length_cloud" soluções */
    starting_cloud();

    while(psa_current_temperature >= psa_final_temperature)
    {
        /* Percorre a nuvem de soluções e gera vizinhos para cada Solução */
        for (list<Solution>::iterator it = cloud_solutions.begin(); it != cloud_solutions.end(); it++)
        {
            /* Gera Vizinho */
            psa_solution_neighbor = neighborhood_search(*it);

            /* Gera resultados na simulação */
            run_simulation(psa_solution_neighbor);
        }
        /* Decaimento da Temperatura*/
        psa_current_temperature *= psa_alpha_temperature;
    }
}

Solution random_neighbor(Solution base)
{
    Solution random_solution = base;

    unsigned int r1 = ((rand()%98)+1);
    unsigned int r2 = (((rand()%(99-r1))+1));
    unsigned int r3 = 100 - (r1+r2);

    random_solution.scale_wcrt            = (double) (r1/100);
    random_solution.scale_frames_burst    = (double) (r2/100);
    random_solution.scale_time_mean_burst = (double) (r3/100);

    for (unsigned int i=0; i < length_frames; i++)
    {
        random_solution.candb_solution[i].delay_time = (double)(rand()%((int) random_solution.candb_solution[i].deadline_time));
    }
    run_simulation(random_solution);
    return random_solution;
}

void starting_cloud()
{
    cloud_solutions.push_back(psa_solution_current);
    for (unsigned int i=0; i < length_cloud-1; i++)
    {
        cloud_solutions.push_back(random_neighbor(psa_solution_current));
    }
}

void run_simulation(Solution &solucao)
{
    main_simulated(psa_arq_times_simulation, TIME_SIMULATION);
    solucao.wcrt            =  wcrt;
    solucao.frames_burst    =  max_length_queue;
    solucao.time_mean_burst =  time_mean_queue;
}

Solution neighborhood_search(Solution solucao)
{
    Solution new_solution = solucao;

    for (unsigned int i=0; i < length_frames; i++)
    {
        new_solution.candb_solution[i].delay_time += (double)((rand()%11)-5);
        if (new_solution.candb_solution[i].delay_time < 0)
            new_solution.candb_solution[i].delay_time = 0;

        if (new_solution.candb_solution[i].delay_time > new_solution.candb_solution[i].deadline_time-1)
            new_solution.candb_solution[i].delay_time = new_solution.candb_solution[i].deadline_time-1;
    }

    return new_solution;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    return 0;
}
