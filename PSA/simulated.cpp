#include "simulated.hpp"

using namespace std;

void pareto_simulated_anneling()
{
    /* Temperatura atual do PSA */
    double psa_current_temperature = psa_starting_temperature;
    /* Preenche nuvem com "length_cloud" soluções */
    starting_cloud();

    ofstream init;
    init.open("result_inicial.txt");
    for (Solution& e: cloud_solutions)
    {
        run_simulation(e);
        init << e.wcrt << '\t' << e.time_mean_burst << '\n';
    }
    init.close();

    /* Lista auxiliar para criar a nova nuvem de soluções*/
    list<Solution> list_aux;

    while(psa_current_temperature > psa_final_temperature)
    {
        printf("Temperatura \t %.2f\n", psa_current_temperature);
        list_aux.clear();

        /* Percorre a nuvem de soluções e gera vizinhos para cada Solução */
        for (list<Solution>::iterator it = cloud_solutions.begin(); it != cloud_solutions.end(); it++)
        {
            /* Gera Vizinho */
            psa_solution_neighbor = neighborhood_search(*it);
            /* Gera resultados na simulação */
            run_simulation(psa_solution_neighbor);
            /* Verifica se é uma solução dominate */
            if (is_dominat(psa_solution_neighbor, *it))
            {
                /* Se for dominate atualiza nas atuais e coloca no conjunto de solução fronteiras */
                list_aux.push_back(psa_solution_neighbor);
                add_cloud_frontier(psa_solution_neighbor);
                // std::cout << "melhor achado" << '\n';
            }else{
                /* Caso não, ajusta escalas */
                adjusts_scale(psa_solution_neighbor, *it);
                /* Faça a probabilidade, para poder ser aceita como atual (Não adiciona nas fronteiras)*/
                if (accept(*it, psa_solution_neighbor, psa_current_temperature))
                {
                    list_aux.push_back(psa_solution_neighbor);
                    // std::cout << "sim" << '\n';
                }
                else
                {
                    list_aux.push_back(*it);
                    // std::cout << "nao" << '\n';
                }
            }
        }
        /* Atualiza a nova nuvem de soluções*/
        cloud_solutions = list_aux;
        /* Decaimento da Temperatura*/
        psa_current_temperature *= psa_alpha_temperature;
    }
    ofstream out;
    out.open("result_final.txt");
    for (Solution e: cloud_frontier)
      out << e.wcrt << '\t' << e.time_mean_burst << '\n';
    out.close();
    out.open("result_fronteira.txt");
    for (Solution e: get_best_frontier())
      out << e.wcrt << '\t' << e.time_mean_burst << '\n';
    out.close();
    write_frontier_solutions(get_best_frontier());
}

list<Solution> get_best_frontier()
{
    /* Lista com soluções dominantes. (Ninguem as dominam) */
    list<Solution> dominats;
    /* var. que indica se alguem o dominou */
    bool dominat = true;
    for (list<Solution>::iterator it = cloud_frontier.begin(); it != cloud_frontier.end(); it++)
    {
      for (list<Solution>::iterator it2 = cloud_frontier.begin(); it2 != cloud_frontier.end(); it2++)
          /* Verifica se alguem o domina (it) */
          if (is_dominat(*it2,*it))
          {
              /* Caso seja dominado, não faz parte da melhor fronteira encontrada */
              dominat = false;
              break;
          }
      /* Caso ninguem o domina, é incluido na lista*/
      if (dominat)
          dominats.push_front(*it);

      dominat = true;
    }
    return dominats;
}

void add_cloud_frontier(Solution new_s)
{
    /* adiciona na lista de soluções*/
    cloud_frontier.push_front(new_s);
    /* caso o limite seja ultrapassado, é removido o mais dominado (criterio para desempate, mais velho é removido)*/
    if (cloud_frontier.size() > length_frontier)
    {
        /* Interator com referencia com o mais dominado */
        list<Solution>::iterator dominated;
        /* Var. para encontrar o mais dominado */
        unsigned int more_dominated = 0;
        unsigned int cont           = 0;
        for (list<Solution>::iterator it = cloud_frontier.begin(); it != cloud_frontier.end(); it++)
        {
            for (list<Solution>::iterator it2 = cloud_frontier.begin(); it2 != cloud_frontier.end(); it2++)
                if (is_dominat(*it2, *it))
                   cont++;

            if (cont >= more_dominated)
            {
               more_dominated = cont;
               dominated      = it;
            }

            cont = 0;
        }
        cloud_frontier.erase(dominated);
    }
}

void adjusts_scale(Solution& new_s, Solution base_s)
{
    if (new_s.wcrt > base_s.wcrt)
        new_s.scale_wcrt *= psa_alpha_scale;
    else
        new_s.scale_wcrt /= psa_alpha_scale;

    if (new_s.frames_burst > base_s.frames_burst)
        new_s.scale_frames_burst *= psa_alpha_scale;
    else
        new_s.scale_frames_burst /= psa_alpha_scale;

    if (new_s.time_mean_burst > base_s.time_mean_burst)
        new_s.scale_time_mean_burst *= psa_alpha_scale;
    else
        new_s.scale_time_mean_burst /= psa_alpha_scale;

    double norm = new_s.scale_wcrt;
    norm       += new_s.scale_frames_burst;
    norm       += new_s.scale_time_mean_burst;

    new_s.scale_wcrt            /= norm;
    new_s.scale_frames_burst    /= norm;
    new_s.scale_time_mean_burst /= norm;
}

bool accept(Solution s1, Solution s2, double temp)
{
    double minimo = min<double>(((s1.wcrt-s2.wcrt)/(temp*s2.scale_wcrt)),((s1.time_mean_burst-s2.time_mean_burst)/(temp*s2.scale_time_mean_burst)));
    // minimo = min<double>(minimo, (s2.scale_time_mean_burst*(s1.time_mean_burst-s2.time_mean_burst)/temp));
    double sort = ((double)rand()/RAND_MAX);
    // std::cout << sort << " = contra = " << min<double>(1,exp(minimo)) << '\n';
    return ((min<double>(1,exp(minimo))) > sort);
}

bool is_dominat(Solution s1, Solution s2)
{
    return (
              ((s1.wcrt <= s2.wcrt) && (s1.time_mean_burst <= s2.time_mean_burst))
                                    &&
              ((s1.wcrt <  s2.wcrt) || (s1.time_mean_burst <  s2.time_mean_burst))
           );
}

void random_scale(Solution& solucao)
{
    double r1 = (double)((rand()%98)+1);
    double r2 = (double)(((rand()%(99-(int)(r1)))+1));
    double r3 = 100-(r1+r2);

    solucao.scale_wcrt            = (double) (r1/100);
    solucao.scale_frames_burst    = (double) (r2/100);
    solucao.scale_time_mean_burst = (double) (r3/100);
}

void starting_cloud()
{
    // FIRST !!
    random_scale(psa_solution_first);
    cloud_solutions.push_back(psa_solution_first);

    for (unsigned int i=1; i < length_cloud; i++)
    {
        cloud_solutions.push_back(random_neighbor(psa_solution_first));
    }
}

void run_simulation(Solution& solucao)
{
    simulator = new Simulator_CAN(solucao.candb_solution, length_frames);
    simulator->run_simulation(TIME_SIMULATION);
    // double delay_sum = 0;
    // for (size_t i = 0; i < length_frames; i++)
    // {
    //     delay_sum += solucao.candb_solution[i].delay_time;
    // }
    // delay_sum *= 100;
    solucao.wcrt            =  simulator->wcrt;
    // solucao.wcrt            =  rand()%1000;
    solucao.frames_burst    =  simulator->frames_burst;
    // solucao.frames_burst    =  rand()%100000;
    solucao.time_mean_burst =  simulator->time_mean_burst;
    // solucao.time_mean_burst =  rand()%1000;
    delete(simulator);
}

Solution random_neighbor(Solution base)
{
    Solution random_solution = base;
    random_solution.candb_solution = new Frame_CAN[length_frames];

    for (unsigned int i=0; i < length_frames; i++)
    {
        random_solution.candb_solution[i] = base.candb_solution[i];
        random_solution.candb_solution[i].delay_time = (double)(rand()%((int) random_solution.candb_solution[i].deadline_time));
    }
    random_scale(random_solution);
    return random_solution;
}

Solution neighborhood_search(Solution solucao)
{
    Solution new_solution = solucao;
    new_solution.candb_solution = new Frame_CAN[length_frames];

    for (unsigned int i=0; i < length_frames; i++)
    {
        new_solution.candb_solution[i] = solucao.candb_solution[i];
        new_solution.candb_solution[i].delay_time += (double)((rand()%11)-5);
        if (new_solution.candb_solution[i].delay_time < 0)
            new_solution.candb_solution[i].delay_time = 0;

        if (new_solution.candb_solution[i].delay_time > new_solution.candb_solution[i].deadline_time-1)
            new_solution.candb_solution[i].delay_time = new_solution.candb_solution[i].deadline_time-1;
    }

    return new_solution;
}

void write_frontier_solutions(list<Solution> frontier)
{
    FILE** files = (FILE**) malloc(sizeof(FILE*)*(frontier.size()));
    char path[64];

    list<Solution>::iterator it = frontier.begin();

    for (size_t i = 0; i < (frontier.size() && (i < 6)); i++)
    {
        sprintf(path, "best-%d.txt", i);
        files[i] = fopen(path, "w+");
        fprintf(files[i], "WCRT = %.4f\nTIME_MEAN_BURST = %.4f\n\n", it->wcrt, it->time_mean_burst);
        for (size_t j = 0; j < length_frames; j++)
        {
            printf("%u\t%.2f\t%.2f\n", it->candb_solution[j].id, it->candb_solution[j].cycle_time, it->candb_solution[j].delay_time);
            fprintf(files[i], "%u\t%.2f\t%.2f\n", it->candb_solution[j].id, it->candb_solution[j].cycle_time, it->candb_solution[j].delay_time);
        }
        fclose(files[i]);
        it++;
    }

}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    FILE* arq  = fopen(argv[1], "r");
    if (!arq)
    {
        std::cerr << "\nERRO AO ABRIR O ARQUIVO\n" << '\n';
        exit(3);
    }
    Frame_CAN* frames = get_CANDB(arq, length_frames);

    psa_solution_first.candb_solution = frames;
    psa_starting_temperature = 8000;
    psa_final_temperature     = 10;
    psa_alpha_temperature    = 0.9;
    psa_alpha_scale          = 0.1;
    length_cloud             = 300;
    length_frontier          = 150;
    pareto_simulated_anneling();

    //
    // std::cout << "WCRT > " << sim.wcrt << "\nTIME_BURST > " << sim.time_mean_burst
    //           << "\nFrames Burst > " << sim.frames_burst
    //           << "\nDeadLines > " << sim.deadlines << '\n';

    return 0;
  }
