#ifndef  SIMULATOR
#define   SIMULATOR

#include <cmath>
#include <list>
#include <iostream>

#define     DURATION_FRAME       (double)(134/(pow(2,20)/1000))
#define     ERRO_INPUT_CANDB     13

/******************************************************************

Estrutura (classe) para representar um frame CAN

*******************************************************************/

class Frame_CAN
{
  public:
      unsigned int  id:11;               // ID da mensagem
      double        cycle_time;          // Periodo de ciclo
      double        delay_time;          // Tempo proposto para start delay (offset)
      double        deadline_time;       // Tempo de limite maximo em fila
};


class Event
{
  public:
      double        time_intended;    // Tempo no qual o frame ira concorrer pelo barramento
      double        time_happened;    // Tempo real de acesso ao meio
      double        duration;         // Tempo de duração de escrita no barramento
      double        wcrt;             // Pior tempo de fila encontrado por esse frame
      Frame_CAN     frame;            // Dados do frame

      bool operator==(Event e) const{
          return (this->frame.id == e.frame.id);
      }
};


class Simulator_CAN
{
  public:
      std::list<Event>  event_list;
      double            wcrt;
      double            time_mean_burst;
      double            frames_burst;
      unsigned int      deadlines;

      Simulator_CAN(Frame_CAN* frames, int length);
      void run_simulation(double time_simulation);
  private:
     void add_event(Event new_event);
     void calc_time_burst(double time_current, unsigned int length_queue);
};


std::list<Event>::iterator get_priority_id(double time_current, std::list<Event>& event_list);
Frame_CAN* get_CANDB(FILE* candb, unsigned int& length);

#endif
