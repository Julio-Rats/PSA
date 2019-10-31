#include "simulated.hpp"

using namespace std;

void pareto_simulated_anneling()
{
    /* Temperatura atual do PSA */
    double psa_current_temperature = psa_starting_temperature;
    /* Preenche nuvem com "length_cloud" soluções */
    starting_cloud();
    /* Lista auxiliar para criar a nova nuvem de soluções*/
    vector<Solution> list_aux;

    while(psa_current_temperature > psa_final_temperature)
    {
        // printf("Temperatura \t %.2f\n", psa_current_temperature);
        for (size_t i = 0; i < psa_number_neighbor; i++)
        {
          list_aux.clear();
          /* Percorre a nuvem de soluções e gera vizinhos para cada Solução */
          for (vector<Solution>::iterator it = cloud_solutions.begin(); it != cloud_solutions.end(); it++)
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
              }
              else
              {
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
        }
        /* Decaimento da Temperatura*/
        psa_current_temperature *= psa_alpha_temperature;
    }
}

void add_cloud_frontier(Solution new_s)
{
    /* adiciona na lista de soluções*/
    cloud_frontier.push_back(new_s);
    /* caso o limite seja ultrapassado, é removido o mais dominado (criterio para desempate, mais velho é removido)*/
    if (cloud_frontier.size() > length_frontier)
    {
        // /* Interator com referencia com o mais dominado */
        vector<Solution>::iterator dominated;
        /* Var. para encontrar o mais dominado */
        unsigned int less_domineering = 0;
        unsigned int cont             = 0;
        for (vector<Solution>::iterator it = cloud_frontier.begin(); it != cloud_frontier.end(); it++)
        {
            for (vector<Solution>::iterator it2 = cloud_frontier.begin(); it2 != cloud_frontier.end(); it2++)
                if (is_dominat(*it, *it2))
                   cont++;

            if (cont <= less_domineering)
            {
               less_domineering = cont;
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
    minimo = min<double>(minimo, ((s1.frames_burst-s2.frames_burst)/(temp*s2.scale_frames_burst)));
    double sort = ((double)rand()/RAND_MAX);
    // std::cout << sort << " = contra = " << min<double>(1,exp(minimo)) << '\n';
    return ((min<double>(1,exp(minimo))) > sort);
}

bool is_dominat(Solution s1, Solution s2)
{
    return (
              ((s1.wcrt <= s2.wcrt) && (s1.time_mean_burst <= s2.time_mean_burst) && (s1.frames_burst <= s2.frames_burst))
                                    &&
              ((s1.wcrt <  s2.wcrt) || (s1.time_mean_burst <  s2.time_mean_burst) || (s1.frames_burst < s2.frames_burst))
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
    if (solucao.wcrt == 0)
        printf("ACHHHOUUUUU WCRT == 0\n");
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

vector<vector<Solution>> get_frontiers()
{
    for (Solution& person: cloud_frontier)
        run_simulation(person);

    vector<vector<Solution>> frontiers(1);

    for (Solution& person_dmt: cloud_frontier)
    {
        for (Solution& person_slv: cloud_frontier)
        {
            if (is_dominat(person_dmt, person_slv))
            {
                person_dmt.dominated.push_back(&person_slv);
            }
            else if (is_dominat(person_slv, person_dmt))
            {
                person_dmt.transcend++;
            }
        }
        if (person_dmt.transcend == 0)
        {
            frontiers[0].push_back(person_dmt);
        }
    }
    unsigned int i = 0;
    vector<Solution> aux_dominates;
    while (frontiers[i].size() != 0)
    {
        aux_dominates.clear();
        for (Solution& person_frt: frontiers[i])
          for (Solution* person_dmtd: person_frt.dominated)
          {
              person_dmtd->transcend--;
              if (person_dmtd->transcend == 0)
              {
                  aux_dominates.push_back(*person_dmtd);
              }
          }
        frontiers.push_back(aux_dominates);
        i++;
    }
    frontiers.resize(frontiers.size()-1);
    return frontiers;
}

void write_arq(vector<vector<Solution>> frontiers, const char *path1, const char *path2)
{
    FILE* archive_front = fopen(path1,"w");
    if (!archive_front)
    {
        printf("\n\nERRO ao abrir o aquivo fronteiras.txt\n\n");
        exit(1);
    }
    fprintf(archive_front,"WCRT\tBRUST_SIZE\tTIME_BURST\tN_FRONT\n");
    for (size_t i = 0; i < frontiers.size(); i++)
      for (Solution& person: frontiers[i])
      {
          fprintf(archive_front, "%f\t%f\t%f\t%d\n", person.wcrt, person.frames_burst,
                    person.time_mean_burst, i);
      }
      fclose(archive_front);

      for (size_t i = 0; i < frontiers[0].size(); i++)
      {
          char path_results[64]="";
          sprintf(path_results, "%s-%d.txt", path2, i);

          FILE* out = fopen(path_results, "w+");
          if (!out)
          {
              std::cout << "\n[ERRO:] Ao criar arquivo de resultados" << "\n\n";
              exit(10);
          }
          fprintf(out, "WCRT\t%lf\n",            frontiers[0][i].wcrt);
          fprintf(out, "TIME BURST\t%lf\n",      frontiers[0][i].time_mean_burst);
          fprintf(out, "SIZE BURST\t%lf\n\n",    frontiers[0][i].frames_burst);
          fprintf(out, "ID_MSG\tCYCLE\tDEADLINE\tSTART_DLY\tPAYLOAD\n");
          for (size_t j = 0; j < length_frames; j++)
              fprintf(out,"%u\t%lf\t%lf\t%lf\t%u\n", frontiers[0][i].candb_solution[j].id, frontiers[0][i].candb_solution[j].cycle_time,
                      frontiers[0][i].candb_solution[j].deadline_time, frontiers[0][i].candb_solution[j].delay_time,
                      frontiers[0][i].candb_solution[j].payload_frame);

          fclose(out);
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
    psa_final_temperature    = 10;
    psa_alpha_temperature    = 0.95;
    psa_alpha_scale          = 0.15;
    psa_number_neighbor      = 10;
    length_cloud             = 80;
    length_frontier          = 100;

    pareto_simulated_anneling();

    write_arq(get_frontiers(), argv[2], argv[3]);

    return 0;
  }
