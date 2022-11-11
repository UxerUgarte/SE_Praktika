
#include "datu_egiturak.h"
#include "circular_queue.h"


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond1  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond2  = PTHREAD_COND_INITIALIZER;

sem_t sem_scheduler;
sem_t sem_pr_sor;

struct tenp *tenp_prsor;
struct tenp *tenp_sch;

int done = 0;
int tenp_kop = 0;

int prozesu_id = 1;

struct pcb *prozesu_nulua;
struct pcb *executing;

struct node *processQueue;

int executed = 0;
int lagExecuted = 0;

int terminated = 0;

float denb = 0.0;

int sch;

/**
 * Erlojuaren funtzioa. Tenporizadoreekin sinkronizatzen da.
 */
void *erlojua() {

    printf("Erlojua aktibatuta.\n");
    
    while(1){
        //Mutex-a blokeatu
        pthread_mutex_lock( &mutex1 );

        //Tenporizadore guztientzat
        while (done < tenp_kop){
            //Tenporizadorea baliabidea kontsumitzeko zain geratu
            pthread_cond_wait( &cond1, &mutex1 );
        }
        //sleep(1);
        done = 0;
        //Hari guztiei seinalea bidali hauek desblokeatzeko
        pthread_cond_broadcast(&cond2);
        //Mutex-a desblokeatu
        pthread_mutex_unlock(&mutex1);
    }
}

/**
 * Prozesu sortzailearen tenporizadorea. Prozesu sortzaileari seinaleak bidaltzen dizkio. 
 */
void *tenporizadorea_prsor() {

    int kont = 0;
    pthread_mutex_lock( &mutex1 );
    printf("Prozesu Sortzailearen tenporizadorea aktibatuta.\n");

    

    while(1){

        done++;
        kont++;
        //printf("Kont(Prozesu Sortzailea): %d\n", kont);
        if (kont == PRSOR_FREC){
            printf("(Tenp Prozesu Sortzailea): Sartu naiz. Prozesu sortzaileari seinalea bidaltzen...\n");
            kont = 0;
            //Semaforoari seinalea bidali 
            sem_post(&sem_pr_sor);
        }
        
        //Erlojuari seinalea bidali erlojua desblokeatzeko
        pthread_cond_signal(&cond1);
        //Erlojuaren seinaleari zain geratu
        pthread_cond_wait( &cond2, &mutex1 );
    }
}


/**
 * Schedulerraren tenporizadorea. Schedulerrari seinaleak bidaltzen dizkio.
 */
void *tenporizadorea_scheduler() {

    int kont = 0;
    pthread_mutex_lock( &mutex1 );
    printf("Schedulerraren tenporizadorea aktibatuta\n");

    while(1){

        done++;
        kont++;
        //printf("Kont(Scheduler): %d\n", kont);
        if (kont == SCH_FREC || executed){
            printf("(Tenp Scheduler): Sartu naiz kontagailu honekin %d. Schedulerrari seinalea bidaltzen...\n", kont);
            kont = 0;
            executed = 0;
            //Semaforoari seinalea bidali
            sem_post(&sem_scheduler);
        }
        //Erlojuari seinalea bidali erlojua desblokeatzeko
        pthread_cond_signal(&cond1);
        //Erlojuaren seinaleari zain geratu
        pthread_cond_wait( &cond2, &mutex1 );
    }
}


/**
 * Prozesu sortzailearen tenporizadoreak seinale bat bidaltzean prozesu berri bat sortu eta prozesuen listan sartuko du.
 */
void *prozesu_sortzailea(){
    
    printf("Prozesu sortzailea aktibatuta.\n");

    while(1){

        //Prozesu sortzailearen tenporizadorearen semaforoari zain geratu
        sem_wait(&sem_pr_sor);

        printf("(Prozesu sortzailea): Seinalea jasota. Nire gauzak egiteko prest...\n");

        //Prozesu berria sortu
        struct pcb *newPcb = (struct pcb*)malloc(sizeof(struct pcb));  //Memoria alokatu
        newPcb->ID = prozesu_id;
        newPcb->STATE = READY;
        newPcb->CPU_TIME = 0;
        float r =rand()%40+1;
        //printf("Beharrezko denbora: %f\n", r);
        newPcb->beharrezko_denbora = r;
        prozesu_id++;                                                  //Prozesu kopurua handitu
        pthread_mutex_lock( &mutex2 );
        insertFirst(&processQueue,  newPcb);                           //Prozesua ilaran sartu
        pthread_mutex_unlock( &mutex2 );
        if(executing->ID == 0){
            executed = 1;
        }
        
        printProcessQueue(processQueue);
        
    }
}

/**
 * Exekuzioa simulatuko duen funtzioa
 */ 
void *execution(){
    
    //Zein prozesu exekutatzen ariden 
    printf("%d prozesua exekutatzen\n", executing->ID);
    printf("CPU denbora: %f\n", executing->CPU_TIME);
    printf("beharrezko denbora: %f\n", executing->beharrezko_denbora);
    if(executing->beharrezko_denbora > executing->CPU_TIME){
        sleep(executing->beharrezko_denbora - executing->CPU_TIME);
    }
    //Schedulerraren tenporizadorea berrabiarazi eta schedulerrari jakinari prozesu honek bukatu duela
    executed = 1;
    lagExecuted = 1;
}

/**
 * Roud Robin algoritmoa simulatzen duen funtzioa
 * param exec exekuzioa simulatuko duen haria
 * param start hasierako denbora lortzeko erabiliko den aldagaia
 * param end bukaerako denbora hartzeko erabiliko den aldagaia
 * currenNode processQueueko uneko nodoa
 */
void RR(pthread_t exec, clock_t start, clock_t end, struct node *currentNode){
    //Aldatu egoera
        pthread_mutex_lock( &mutex2 );
        if (currentNode->next != currentNode)
        {   
            currentNode = currentNode->next;
            if(currentNode->data->ID == 0){
                currentNode = currentNode->next;
            }
            currentNode->data->STATE = EXECUTING;
            executing = currentNode->data;
        }else{
            executing = prozesu_nulua;
            executing->beharrezko_denbora = 1000000000;
            executing->CPU_TIME = 0;
            executing->STATE = EXECUTING;
        }
        pthread_mutex_unlock( &mutex2 );

        //Exekutatu
        start = clock();

        pthread_create( &exec, NULL, execution, NULL);

        //Schedulerraren tenporizadorearen semaforoari zain geratu
        sem_wait(&sem_scheduler);



        end = clock();

        denb = (float)(end - start) / CLOCKS_PER_SEC;

        printf("%d Prozesuak erabilitako denbora: %f \n", executing->ID, denb);

        //Neurtutako denbora ez da zehatza. Horregatik dago if hau
        if(denb+executing->CPU_TIME > executing->beharrezko_denbora){
            lagExecuted = 1;
            printf("%d prozesua bukatu da\n", executing->ID);
        }

        pthread_mutex_lock( &mutex2 );
        //Prozesua listatik kendu jada exekutatu bada
        if(lagExecuted){
            deleteByNode(processQueue, currentNode);
        }else{
            executing->CPU_TIME = executing->CPU_TIME + denb;
            executing->STATE = READY;
            
        }
        pthread_mutex_unlock( &mutex2 );
        pthread_cancel(exec);
        lagExecuted = 0;

        printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");

}

/**
 * Fifo algoritmoa simulatzen duen funtzioa. 0 prozesua bada schedulerraren seinalearen zain egongo da. Bestela prozesuak bukatzen duen arte 
 * geratuko da zain.
 * param exec exekuzioa simulatuko duen haria
 * param start hasierako denbora lortzeko erabiliko den aldagaia
 * param end bukaerako denbora hartzeko erabiliko den aldagaia
 * currenNode processQueueko uneko nodoa
 */
void fifo(pthread_t exec, clock_t start, clock_t end, struct node *currentNode){
        //Aldatu egoera
        pthread_mutex_lock( &mutex2 );
        if (currentNode->next != currentNode)
        {           
            currentNode = currentNode->next;
            if(currentNode->data->ID == 0){
                currentNode = currentNode->next;
            }
            currentNode->data->STATE = EXECUTING;
            executing = currentNode->data;
        }else{
            executing = prozesu_nulua;
            executing->beharrezko_denbora = 1000000000;
            executing->CPU_TIME = 0;
            executing->STATE = EXECUTING;
        }
        pthread_mutex_unlock( &mutex2 );

        //Exekutatu
        start = clock();

        pthread_create( &exec, NULL, execution, NULL);
        if(executing->ID != 0){
            pthread_join(exec, NULL);
        }else{
            //Schedulerraren tenporizadorearen semaforoari zain geratu 0 prozesua bada
            sem_wait(&sem_scheduler);
        }

        end = clock();

        denb = (float)(end - start) / CLOCKS_PER_SEC;

        printf("%d Prozesuak erabilitako denbora: %f \n", executing->ID, denb);

        //Neurtutako denbora ez da zehatza. Horregatik dago if hau
        if(denb+executing->CPU_TIME > executing->beharrezko_denbora){
            lagExecuted = 1;
            printf("%d prozesua bukatu da\n", executing->ID);
        }
        
        pthread_mutex_lock( &mutex2 );
        //Prozesua listatik kendu jada exekutatu bada
        if(lagExecuted){
            deleteByNode(processQueue, currentNode);
        }else{
            executing->CPU_TIME = executing->CPU_TIME + denb;
            executing->STATE = READY;
            
        }
        pthread_mutex_unlock( &mutex2 );
        pthread_cancel(exec);
        lagExecuted = 0;

        printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");

}



/**
 * Schedulerraren funtzioa. Oraingoz schedulerraren tenporizadoreak bidali behar dion seinalearen zain dago round robin exekutatzen duenean. Fifo exekutatzen badu 
 * ez da egongo schedulerraren tenporizadorearen zain.
 */
void *scheduler(){
 
    struct node *currentNode;
    pthread_t exec;
    clock_t start, end;

    printf("Schedulerra aktibatuta.\n");

    //pthread_create( &exec, NULL, execution, NULL);

    currentNode = processQueue;

    int i = 0;
    
    while(1){

        //Schedulerrak egin behar dituen gauzak hemen

        switch(sch){
            case FIFO_SCH:
                // Fifo exekutatu
                fifo(exec, start, end, currentNode);
                break;
            case RR_SCH:
                // RoundRobin exekutatu
                //Aldatu egoera
                pthread_mutex_lock( &mutex2 );
                if (currentNode->next != currentNode)
                {   
                    currentNode = currentNode->next;
                    if(currentNode->data->ID == 0){
                        currentNode = currentNode->next;
                    }
                    currentNode->data->STATE = EXECUTING;
                    executing = currentNode->data;
                }else{
                    executing = prozesu_nulua;
                    executing->beharrezko_denbora = 1000000000;
                    executing->CPU_TIME = 0;
                    executing->STATE = EXECUTING;
                }
                pthread_mutex_unlock( &mutex2 );

                //Exekutatu
                start = clock();

                pthread_create( &exec, NULL, execution, NULL);

                //Schedulerraren tenporizadorearen semaforoari zain geratu
                sem_wait(&sem_scheduler);



                end = clock();

                denb = (float)(end - start) / CLOCKS_PER_SEC;

                printf("%d Prozesuak erabilitako denbora: %f \n", executing->ID, denb);

                //Neurtutako denbora ez da zehatza. Horregatik dago if hau
                if(denb+executing->CPU_TIME > executing->beharrezko_denbora){
                    lagExecuted = 1;
                    printf("%d prozesua bukatu da\n", executing->ID);
                }

                //
                pthread_mutex_lock( &mutex2 );
                //Prozesua listatik kendu jada exekutatu bada
                if(lagExecuted){
                    deleteByNode(processQueue, currentNode);
                }else{
                    executing->CPU_TIME = executing->CPU_TIME + denb;
                    executing->STATE = READY;
                    
                }
                pthread_mutex_unlock( &mutex2 );
                pthread_cancel(exec);
                lagExecuted = 0;

                printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");
                break;
            default:
                // RoundRobin exekutatu
                //Aldatu egoera
                pthread_mutex_lock( &mutex2 );
                if (currentNode->next != currentNode)
                {   
                    currentNode = currentNode->next;
                    if(currentNode->data->ID == 0){
                        currentNode = currentNode->next;
                    }
                    currentNode->data->STATE = EXECUTING;
                    executing = currentNode->data;
                }else{
                    executing = prozesu_nulua;
                    executing->beharrezko_denbora = 1000000000;
                    executing->CPU_TIME = 0;
                    executing->STATE = EXECUTING;
                }
                pthread_mutex_unlock( &mutex2 );

                //Exekutatu
                start = clock();

                pthread_create( &exec, NULL, execution, NULL);

                //Schedulerraren tenporizadorearen semaforoari zain geratu
                sem_wait(&sem_scheduler);



                end = clock();

                denb = (float)(end - start) / CLOCKS_PER_SEC;

                printf("%d Prozesuak erabilitako denbora: %f \n", executing->ID, denb);

                //Neurtutako denbora ez da zehatza. Horregatik dago if hau
                if(denb+executing->CPU_TIME > executing->beharrezko_denbora){
                    lagExecuted = 1;
                    printf("%d prozesua bukatu da\n", executing->ID);
                }

                pthread_mutex_lock( &mutex2 );
                //Prozesua listatik kendu jada exekutatu bada
                if(lagExecuted){
                    deleteByNode(processQueue, currentNode);
                }else{
                    executing->CPU_TIME = executing->CPU_TIME + denb;
                    executing->STATE = READY;
                    
                }
                pthread_mutex_unlock( &mutex2 );
                pthread_cancel(exec);
                lagExecuted = 0;

                printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");
        }
        
    }
}


int main(int argc, char *argv[]) {

    
    pthread_t tenp_pr_sor, tenp_scheduler, erlj, pr_sor, tscheduler;
    //Fifo edo RoundRobin hautatzeko
    if(argc>1 && strcmp(argv[1], "fifo")==0){
        sch = 0;
    }else{
        sch = 1;
    }

    //Prozesu nulua sortzen
    prozesu_nulua = (struct pcb*)malloc(sizeof(struct pcb));
    prozesu_nulua->ID = 0;
    prozesu_nulua->STATE = READY;
    prozesu_nulua->CPU_TIME = 0;
    prozesu_nulua->beharrezko_denbora = 100000000;
    
    //Prozesu sortzailea eta bere tenporizadorea sinkronizatuta mantenduko duen semaforoa hasieratu
    sem_init(&sem_pr_sor, 0, 0);

    //Schedulerra eta bere tenporizadorea sinkronizatuta mantenduko duen semaforoa hasieratu
    sem_init(&sem_scheduler, 0, 0);

    //Prozesuen ilara zirkularra sortzen sortu
    processQueue = createCircularQueue();

    insertFirst(&processQueue, prozesu_nulua);
    
    //Erlojua sortu
    pthread_create( &erlj, NULL, erlojua, NULL);


    //Tenporizadoreak sortu
    pthread_create( &tenp_pr_sor, NULL, tenporizadorea_prsor, NULL);
    tenp_kop++;
    pthread_create( &tenp_scheduler, NULL, tenporizadorea_scheduler, NULL);
    tenp_kop++;

    //Prozesu sortzailea sortu
    pthread_create( &pr_sor, NULL, prozesu_sortzailea, NULL);

    //Schedulerra sortu
    pthread_create( &tscheduler, NULL, scheduler, NULL);

    pthread_join(erlj, NULL);
}


/*
void *execution(){
    int i;
    clock_t start, end;

    while(1){
        i = 0;
        start = clock();
        while(!terminated && i<executing->beharrezko_denbora){
            sleep(0.1);
            i++;
        }
        printf("beharrezko denbora: %d; %d\n", executing->beharrezko_denbora, i);
        end = clock();
        denb = (float)(end - start) / CLOCKS_PER_SEC;

        if(terminated){
            printf("%d prozesua bukatu da\n", executing->ID);
            //Schedulerraren tenporizadorea berrabiarazi eta schedulerrari jakinari prozesu honek bukatu duela
            executed = 1;

            //Schedulerra aktibatzeko seinalea bidali
            sem_post(&sem_scheduler);

            terminated = 0;


        }
        

        
    }


}
*/