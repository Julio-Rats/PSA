#include "simulador.hpp"

using namespace std;

Simulator_CAN::Simulator_CAN(Frame_CAN* frames, int length)
{
    /* Limpa a lista. */
    this->event_list.clear();
    Event new_event;
    new_event.duration                      = DURATION_FRAME;
    new_event.wcrt                          = 0;
    this->wcrt = this->frames_burst         = 0;
    this->time_mean_burst = this->deadlines = 0;
    /* Adiciona todos os Frames recebidos. */
    for (int i=0; i < length; i++)
    {
        /* Inicializa todos os campos. */
        new_event.time_intended = frames[i].delay_time;
        new_event.time_happened = frames[i].delay_time;
        new_event.frame         = frames[i];
        /* Adiciona na lista. */
        this->add_event(new_event);
    }
}

void Simulator_CAN::run_simulation(double time_simulation)
{
    Event  aux_event;
    list<Event>::iterator prioritary_it;

    unsigned int cont_queue   = 0;
    double       time_current = 0;

    this->wcrt = this->frames_burst = this->time_mean_burst = this->deadlines = 0;

    while (time_current < time_simulation)
    {
        /* Avança do tempo*/
        time_current  = this->event_list.front().time_happened;
        /* Pega o ID mais prioritario de agora*/
        prioritary_it = get_priority_id(time_current, event_list);
        aux_event     = *prioritary_it;
        /* Apagara o evento*/
        this->event_list.erase(prioritary_it);
        /* Calcula o WCRT deste ID*/
        if (aux_event.time_happened != aux_event.time_intended){
            if (aux_event.wcrt < (aux_event.time_happened - aux_event.time_intended))
                aux_event.wcrt = (aux_event.time_happened - aux_event.time_intended);
        }
        /* Verifica se houve um deadline. */
        if (aux_event.frame.deadline_time <= (aux_event.time_happened - aux_event.time_intended))
        {
            this->deadlines++;
        }
        /* Ajusta o tempo do novo evento*/
        aux_event.time_happened += (aux_event.frame.cycle_time + aux_event.duration);
        aux_event.time_intended  = aux_event.time_happened;
        /* Adiciona o evento na lista de eventos*/
        this->add_event(aux_event);
        /* Avança o tempo atual com a duração do frame*/
        time_current += aux_event.duration;
        cont_queue    = 0;
        /* Ajusta os frames concorrentes (LOST ARBITRATION) e verifica filas*/
        for (Event& e: this->event_list)
        {
            if (e.time_happened < time_current){
                e.time_happened = time_current;
                cont_queue++;
              }
            else
                break;
        }
        /* Calcula maior time e frames no burst (fila)*/
        this->calc_time_burst((time_current-aux_event.duration), cont_queue);
        // getchar();
    }
    /* Calculo o pior WCRT (Acumulado de todos so wcrt)*/
    for (Event e: this->event_list)
        this->wcrt +=  e.wcrt;
}

void Simulator_CAN::add_event(Event new_event)
{
    // Adiciona se a fila for vazia
    if (!this->event_list.size()){
        this->event_list.push_front(new_event);
        return;
    }
    // Busca na fila a posição certa para inserção.
    for (list<Event>::iterator it = this->event_list.begin(); it != this->event_list.end(); it++)
    {
        // Verifica se esta posição é maior.
        if (it->time_happened >= new_event.time_intended)
        {
            // Adiciona na posição antes do maior.
            this->event_list.insert(it, new_event);
            return;
        }
    }
    // Caso seja a maior de todas, adiciona no final.
    this->event_list.insert(this->event_list.end(), new_event);
}

void Simulator_CAN::calc_time_burst(double time_current, unsigned int length_queue)
{
    // Quantidade de frames em fila na ultima vez.
    static unsigned int last_queue = 0;
    // Time em que começou atual fila.
    static double       last_time  = 0;
    if (length_queue)
    {
        if (last_queue == 0)
           last_time = time_current;

        if (length_queue > last_queue)
        {
            last_queue = length_queue;
            /* Verifica se esta é a maior fila*/
            if (this->frames_burst < length_queue)
                this->frames_burst = length_queue;
        }
    }
    else
    {
        if (last_queue != 0)
        {
            // if (this->time_mean_burst < (time_current-last_time))
            if ((time_current-last_time) > 0)
                this->time_mean_burst += (time_current-last_time);
            last_queue = 0;
        }
    }
}

list<Event>::iterator get_priority_id(double time_current, list<Event>& event_list)
{
    list<Event>::iterator prioritary_it, it;
    unsigned int          prioritary_id;

    prioritary_id = event_list.front().frame.id;
    prioritary_it = it = event_list.begin();

    for (Event e: event_list)
    {
        if (e.time_happened == time_current)
        {
            if (e.frame.id < prioritary_id)
            {
                prioritary_id = e.frame.id;
                prioritary_it = it;
            }
        }
        else
        {
            break;
        }
        it++;
    }
    return prioritary_it;
}

Frame_CAN* get_CANDB(FILE* candb, unsigned int& length)
{
   unsigned int  id;
   double        cycle_time, deadline_time, delay_time;
   double        cycle_time_fast; // ???????

   Frame_CAN*    frames;
   length = 0;

   try
   {
      while (fscanf(candb,"%u\t%lf\t%lf\t%lf\t%lf\n", &id, &cycle_time, &deadline_time, &delay_time, &cycle_time_fast) != EOF)
      {
          if (frames)
          {
              frames = (Frame_CAN*) realloc(frames, sizeof(Frame_CAN)*(++length));
          }
          else
          {
              frames = (Frame_CAN*) malloc(sizeof(Frame_CAN)*(++length));
          }
          if (!frames)
          {
              std::cout << "\n[ERRO] Alocação de memoria para Frames na função get_CANDB()\n" << '\n';
              exit(ERRO_INPUT_CANDB);
          }
          frames[length-1].id            = id;
          frames[length-1].cycle_time    = cycle_time;
          frames[length-1].deadline_time = deadline_time;
          frames[length-1].delay_time    = delay_time;
      }
   }
   catch(int e)
   {
       std::cout << "\n[ERRO] Leitura de frames do CANDB na função get_CANDB()\n" << '\n';
       exit(ERRO_INPUT_CANDB);
   }
   return frames;
}
