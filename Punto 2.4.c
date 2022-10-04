#include "simlib.h"

#define EVENTO_LLEGADA_TERMINAL_1 1  /* Tipo evento llegada a la cola de la terminal 1*/
#define EVENTO_LLEGADA_TERMINAL_2 2 /* Tipo evento llegada a la cola de la terminal 1*/
#define EVENTO_LLEGADA_RENTA 3  /* Tipo evento llegada a la cola de la tienda de renta de autos*/
#define EVENTO_DESCARGA 4 /* Tipo evento llegada a alguno de los lugares*/
#define EVENTO_FIN_SIMULACION 5  /* Tipo evento fin de la simulacion. */
#define EVENTO_FIN_PARADA 6

#define COLA_TERMINAL_1 1
#define COLA_TERMINAL_2 2
#define COLA_RENTA 3
#define COLA_BUS 4

#define SAMPST_DELAYS 1

#define STREAM_LLEGADA_RENTA 1  /* Numero aleatorio para la llegada entre clientes a la tienda de renta de autos. */
#define STREAM_LLEGADA_TERMINAL_1 2 /* Numero aleatorio para la llegada de clientes a la terminal 1. */
#define STREAM_LLEGADA_TERMINAL_2 3 /* Numero aleatorio para la llegada de clientes a la terminal 2. */
#define STREAM_BAJAR_DESCARGA 4 /* Numero aleatorio para la distribucion uniforma al momento en el que los clientes se bajan del bus. */
#define STREAM_SUBIR_DESCARGA 5 /* Numero aleatorio para la distribucion uniforma al momento en el que los clientes se suben del bus. */
#define STREAM_DECIDIR_TERMINAL 6 /* Numero aleatorio para definir en que terminal se bajara el cliente. */


#define NUMERO_PARADAS 3  /* Maximum number of stations. */
#define TIPO_CLIENTE_TERMINAL 2 

#define ESPERA 5
#define TIEMPO_ENTRE_TIENDA_TERMINAL_1 9
#define TIEMPO_ENTRE_TERMINAL_1_TERMINAL_2 2
#define TIEMPO_ENTRE_TERMINAL_2_TIENDA 9
#define CAPACIDAD_BUS 20
/* Declare non-simlib global variables. */

int   numero_parada, ruta, tipo_llegada, duracion_simulacion, puestos_ocupados,
      estado_paradas[NUMERO_PARADAS+1];
    //   num_tasks[MAX_NUM_JOB_TYPES +1],, job_type, task; num_machines[MAX_NUM_STATIONS + 1]

float media_estre_llegadas[4],media_estre_descarga[5],probabilidad_bajar_terminal[3];
// , prob_distrib_job_type[26],mean_service[MAX_NUM_JOB_TYPES +1][ MAX_NUM_STATIONS + 1];

/* Declare non-simlib functions. */

void llegada_paradero();
void descarga_bus();
void bajar_bus();
void subir_bus();
void reporte();
void inicializarSimulacion();

int main()  /* Main function. */
{

    /* Run the simulation until it terminates after an end-simulation event
       (type EVENT_END_SIMULATION) occurs. */
    inicializarSimulacion();

    do {

        /* Determine the next event. */

        timing();

        /* Invoke the appropriate event function. */

        switch (next_event_type) {
            case EVENTO_LLEGADA_RENTA || EVENTO_LLEGADA_TERMINAL_1 || EVENTO_LLEGADA_TERMINAL_2:
                llegada_paradero();
                break;
            case EVENTO_DESCARGA || EVENTO_FIN_PARADA:
                descarga_bus();
                break;
            case EVENTO_FIN_SIMULACION:
                reporte();
                break;
        }

    /* If the event just executed was not the end-simulation event (type
       EVENT_END_SIMULATION), continue simulating.  Otherwise, end the
       simulation. */

    } while (next_event_type != EVENTO_FIN_SIMULACION);

    return 0;
}

void inicializarSimulacion(){
    media_estre_llegadas[EVENTO_LLEGADA_RENTA] = 4.28;
    media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_1] = 6.0;
    media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_2] = 2.5;

    media_estre_descarga[1] = 0.27;
    media_estre_descarga[2] = 0.40;
    media_estre_descarga[3] = 0.25;
    media_estre_descarga[4] = 0.42;

    probabilidad_bajar_terminal[EVENTO_LLEGADA_TERMINAL_1-1] = 0.583;
    probabilidad_bajar_terminal[EVENTO_LLEGADA_TERMINAL_2-1] = 1.0;

    puestos_ocupados = 0;

    /* Initialize all machines in all stations to the idle state. */
    for (int i  = 1; i <= NUMERO_PARADAS; i++)
        estado_paradas[i] = 0;

    /* Initialize simlib */

    init_simlib();

    /* Set maxatr = max(maximum number of attributes per record, 4) */

    maxatr = 4;  /* NEVER SET maxatr TO BE SMALLER THAN 4. */

    /* Programar las llegadas de los clientes a las colas */

    event_schedule(expon(media_estre_llegadas[EVENTO_LLEGADA_RENTA], STREAM_LLEGADA_RENTA),EVENTO_LLEGADA_RENTA);
    event_schedule(expon(media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_1], STREAM_LLEGADA_TERMINAL_1),EVENTO_LLEGADA_TERMINAL_1);
    event_schedule(expon(media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_2], STREAM_LLEGADA_TERMINAL_2),EVENTO_LLEGADA_TERMINAL_2);

    /* Programar la llegada del bus a la siguiente parada */

    event_schedule(TIEMPO_ENTRE_TIENDA_TERMINAL_1,EVENTO_DESCARGA);

    ruta = 1;

    /* Schedule the end of the simulation.  (This is needed for consistency of
       units.) */
    duracion_simulacion = 80;

    event_schedule(60 * duracion_simulacion, EVENTO_FIN_SIMULACION);

}

void bajar_bus(int estacion)  
{
    /* Check to see whether queue is empty. */


    /* The queue is nonempty, so remove the first customer from the queue,
        register delay, increment the number of customers delayed, and
        schedule departure. */

    if (transfer[2] == estacion){

        list_remove(FIRST, COLA_BUS);
        sampst(sim_time - transfer[1], SAMPST_DELAYS);
        ++num_custs_delayed;
        puestos_ocupados--;
        event_schedule(sim_time + uniform(media_estre_descarga[1],media_estre_descarga[2], STREAM_BAJAR_DESCARGA),EVENTO_DESCARGA);
    }

    if else (transfer[2] == estacion){

        list_remove(FIRST, COLA_BUS);
        sampst(sim_time - transfer[1], SAMPST_DELAYS);
        ++num_custs_delayed;
        puestos_ocupados--;
        event_schedule(sim_time + uniform(media_estre_descarga[1],media_estre_descarga[2], STREAM_SUBIR_DESCARGA),EVENTO_DESCARGA);
    }

    else if (transfer[2] == estacion){

        printf("Este no se baja en las terminales, se baja en la tienda\n");
    }

    else {
        printf("Este no se baja nunca");
    }
        
}


void llegada_paradero()  /* Function to serve as both an arrival event of a job
                             to the system, as well as the non-event of a job's
                             arriving to a subsequent station along its
                             route. */
{
    int station;

    /* Se programa la llegada del siguiente cliente dependiendo de cada cola */

    if (next_event_type == EVENTO_LLEGADA_TERMINAL_1){
        event_schedule(sim_time + expon(media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_1], STREAM_LLEGADA_TERMINAL_1),EVENTO_LLEGADA_TERMINAL_1);
        transfer[1] = sim_time;
        transfer[2] = 0;
        list_file(LAST,EVENTO_LLEGADA_TERMINAL_1);
    }
    else if (next_event_type == EVENTO_LLEGADA_TERMINAL_2){
        event_schedule(sim_time + expon(media_estre_llegadas[EVENTO_LLEGADA_TERMINAL_2], STREAM_LLEGADA_TERMINAL_2),EVENTO_LLEGADA_TERMINAL_2);
        transfer[1] = sim_time;
        transfer[2] = 0;
        list_file(LAST,EVENTO_LLEGADA_TERMINAL_2);
    }
    else if (next_event_type == EVENTO_LLEGADA_RENTA){
        event_schedule(sim_time + expon(media_estre_llegadas[EVENTO_LLEGADA_RENTA], STREAM_LLEGADA_RENTA),EVENTO_LLEGADA_RENTA);
        tipo_llegada = random_integer(probabilidad_bajar_terminal,STREAM_DECIDIR_TERMINAL);
        transfer[1] = sim_time;
        transfer[2] = tipo_llegada;
        list_file(LAST,EVENTO_LLEGADA_RENTA);
    }
}


void descarga()  /* Event function for departure of a job from a particular
                      station. */
{

    if (ruta == EVENTO_LLEGADA_TERMINAL_1){
        
        if (list_size[COLA_BUS] != 0){
            // Bajar personas del bus que bajan en la primera terminal
            bajar_bus();
        }
        else if (list_size[EVENTO_LLEGADA_TERMINAL_1] != 0){
            
            event_cancel(EVENTO_FIN_PARADA);
        } else{
            event_schedule(sim_time+ESPERA,EVENTO_FIN_PARADA);
        }

        
    }
    // int station, job_type_queue, task_queue;

    // /* Determine the station from which the job is departing. */

    // job_type = transfer[3];
    // task     = transfer[4];
    // station  = route[job_type][task];

    // /* Check to see whether the queue for this station is empty. */

    // if (list_size[station] == 0) {

    //     /* The queue for this station is empty, so make a machine in this
    //        station idle. */

    //     --estado_paradas[station];
    //     timest((float) estado_paradas[station], station);
    // }

    // else {

    //     /* The queue is nonempty, so start service on first job in queue. */

    //     list_remove(FIRST, station);

    //     /* Tally this delay for this station. */

    //     sampst(sim_time - transfer[1], station);

    //     /* Tally this same delay for this job type. */

    //     job_type_queue = transfer[2];
    //     task_queue     = transfer[3];
    //     sampst(sim_time - transfer[1], num_stations + job_type_queue);

    //     /* Schedule end of service for this job at this station.  Note defining
    //        attributes beyond the first two for the event record before invoking
    //        event_schedule. */

    //     transfer[3] = job_type_queue;
    //     transfer[4] = task_queue;
    //     event_schedule(sim_time
    //                    + erlang(2, mean_service[job_type_queue][task_queue],
    //                             STREAM_SERVICE),
    //                    EVENT_DEPARTURE);
    // }

    // /* If the current departing job has one or more tasks yet to be done, send
    //    the job to the next station on its route. */

    // if (task < num_tasks[job_type]) {
    //     ++task;
    //     arrive(2);
    // }
}


void reporte()  /* Report generator function. */
{
    int   i;
    float overall_avg_job_tot_delay, avg_job_tot_delay, sum_probs;

    /* Compute the average total delay in queue for each job type and the
       overall average job total delay. */

    fprintf(outfile, "\n\n\n\nJob type     Average total delay in queue");
    overall_avg_job_tot_delay = 0.0;
    sum_probs                 = 0.0;
    for (i = 1; i <= num_job_types; ++i) {
        avg_job_tot_delay = sampst(0.0, -(num_stations + i)) * num_tasks[i];
        fprintf(outfile, "\n\n%4d%27.3f", i, avg_job_tot_delay);
        overall_avg_job_tot_delay += (prob_distrib_job_type[i] - sum_probs)
                                     * avg_job_tot_delay;
        sum_probs = prob_distrib_job_type[i];
    }
    fprintf(outfile, "\n\nOverall average job total delay =%10.3f\n",
            overall_avg_job_tot_delay);

    /* Compute the average number in queue, the average utilization, and the
       average delay in queue for each station. */

    fprintf(outfile,
           "\n\n\n Work      Average number      Average       Average delay");
    fprintf(outfile,
             "\nstation       in queue       utilization        in queue");
    for (j = 1; j <= num_stations; ++j)
        fprintf(outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest(j),
                timest(0.0, -j) / num_machines[j], sampst(0.0, -j));
}

