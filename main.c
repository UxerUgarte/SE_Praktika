
#include "datu_egiturak.h"
#include "circular_queue.h"


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond1  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond2  = PTHREAD_COND_INITIALIZER;

sem_t sem_scheduler;
sem_t sem_pr_sor;

struct tenp *tenp_prsor;
struct tenp *tenp_sch;

int done = 0;
int tenp_kop = 0;

int prozesu_id = 1;

struct node *processQueue;

int PHYSICAL_MEMORY[PHYSICAL_MEMORY_SIZE];

struct core *CORES[1];

int num_cores;

int executed = 0;

pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

int sch = 1;

int coreId = -1;

int prozesu_kop = 0;

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
            //printf("ze ostia gertatzen da\n");
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
 * Prozesuen exekuzioa egiteko funtzioa. 
 * param ID ez da ezertarako erabiltzen baina ez dut ikutu nahi, por si acaso
 */
void *core(void *ID) {
    int registers[15];
    int textLogHas, dataLogHas, pgtPhy, dataPhy, helbKont, textPhy, i, j, kontMax, kontagailua, kont2, lagExecuted;
    long helbDataLog, helbDataLogDecHas;
    int atera;
    int kont = 0;
    pthread_mutex_lock( &mutex3 );
    coreId++;
    int id = coreId;
    pthread_mutex_unlock( &mutex3 );
    char * string = (char * )malloc(8*sizeof( char));
    char * stri = (char * )malloc(6*sizeof( char));
    char * lag = (char * )malloc(8*sizeof( char));
    
    printf("Core aktibatuta.\n");
    printf("core id: %d\n", id);
    pthread_mutex_lock( &mutex1 );
    lagExecuted = -1;
    while(1){
        done++;
        kont++;
        atera = 0;
        dataLogHas = 0x0;
        helbKont = 0x0;
        //Core-a executing egoeran badago prozesua kargatu
        if(CORES[id]->egoera == EXECUTING){
            
            registers[15];
            textLogHas = CORES[id]->executing->MEMORY_MANAGEMENT->code;
            dataLogHas = CORES[id]->executing->MEMORY_MANAGEMENT->datuak;
            //Erregistroak soilik RR erabiltzen bada kargatu behar da
            if(sch == RR_SCH){
                
                if(CORES[id]->executing->CPU_TIME == 1){
                    for(int z = 0; z<15; z++){
                        registers[z] = CORES[id]->executing->registers[z];
                    }
                }
                kontMax = 0x20; //RR-an exekutatuko diren agindu kopuru maximoa
                j = CORES[id]->executing->IR;
                helbKont = CORES[id]->executing->PC;
            }else{ //Bestela balio normalekin jarraitu
                kontMax = dataLogHas;
                j = 0;
                helbKont = 0x0;
            }
            //Helbide logiko eta fisiko guztiak lortu
            pgtPhy = CORES[id]->executing->MEMORY_MANAGEMENT->pgb;
            dataPhy = PHYSICAL_MEMORY[pgtPhy+1];
            helbKont = CORES[id]->executing->PC;
            textPhy = PHYSICAL_MEMORY[pgtPhy];
            printf("\n");
            sprintf(lag, "%.8x", dataLogHas);
            helbDataLog;
            helbDataLogDecHas = strtol(lag, NULL, 16);
            i = 0;
            
            kont2 = 0;
            printf("text fisic: %d\n", textPhy);
            printf("data fisic: %d\n", dataPhy);
            printf("Core %d: %d prozesua exekutatzen\n", id, CORES[id]->executing->ID);
            
        }else{
            //Core-a executing egoeran ez badago prozesu nulua exekutatu
            if(kont == SCH_FREC){
                kont = 0;
                printf("Core %d: Prozesu nulua exekutatzen\n", id);
            }
        }
        // Prozesuaren exekuzioa egin
        while(helbKont < dataLogHas && CORES[id]->egoera == EXECUTING && kont2 !=kontMax){
            //Erlojuarekin sinkronizatzeko
            if(atera){
                done++;
                kont++;
            }
            if (kont == CORE_FREC || (CORES[id]->sartu)){

                CORES[id]->sartu = 0;
                kont = 0;
                
                sprintf(string, "%.8x", PHYSICAL_MEMORY[textPhy+j]);
                printf("%s\n", string);
                switch (string[0])
                {
                case '0':
                    /* load agindua */
                    printf("Core %d: ld ", id);
                    printf("r%c ", string[1]);
                    sprintf(stri, "%.*s", 6, string +2);
                    helbDataLog = strtol(stri, NULL, 16);
                    printf("%s\n", stri);
                    //printf("%ld\n", helbDataLog);
                    i = (helbDataLog - helbDataLogDecHas)/4 + dataPhy ;
                    printf("Core %d: Lortutako mem helb fisikoa: %d\n", id, i);
                    printf("Core %d: Lortutako balioa: %d\n", id, (__int32_t)PHYSICAL_MEMORY[i]);
                    registers[string[1]-48] = (__int32_t)PHYSICAL_MEMORY[i];
                    break;
                case '1':
                    /* store agindua */
                    printf("Core %d: st ", id);
                    printf("r%c ", string[1]);
                    sprintf(stri, "%.*s", 6, string +2);
                    helbDataLog = strtol(stri, NULL, 16);
                    printf("%s\n", stri);
                    i = (helbDataLog - helbDataLogDecHas)/4 + dataPhy ;
                    printf("Core %d: Lortutako mem helb fisikoa: %d\n", id, i);
                    printf("Core %d: Lortutako balioa: %d\n", id, registers[string[1]-48]);
                    PHYSICAL_MEMORY[i] = registers[string[1]-48];
                    break;
                case '2':
                    /* add agindua */
                    printf("Core %d: add\n", id);
                    printf("Core %d: Lortutako balioa r%ctik: %d\n", id, string[2], registers[string[2]-48]);
                    printf("Core %d: Lortutako balioa r%ctik: %d\n", id, string[3],registers[string[3]-48]);
                    registers[string[1]-48] = registers[string[2]-48] + registers[string[3]-48];
                    printf("Core %d: Emaitza r%ctik: %d\n", id, string[1],registers[string[1]-48]);
                    break;
                default:
                    /* exit agindua */

                    pthread_mutex_lock( &mutex2 );
                    //Prozesua listatik kendu jada exekutatu bada
                    deleteByNode(processQueue, CORES[id]->unekoNodo);
                    printProcessQueue(processQueue);
                    CORES[id]->executing->STATE = READY;
                    CORES[id]->egoera = READY;
                    pthread_mutex_unlock( &mutex2 );
                    prozesu_kop--;
                    lagExecuted = 1;

                    printf("Core %d: %d prozesua exekutatu dut\n", id, CORES[id]->executing->ID);
                    printf("Core %d: %d prozesua exekutatu dut\n", id, CORES[id]->executing->ID);
                    printf("Core %d: %d prozesua exekutatu dut\n", id, CORES[id]->executing->ID);
                    printf("Core %d: %d prozesua exekutatu dut\n", id, CORES[id]->executing->ID);
                    printf("Core %d: %d prozesua exekutatu dut\n", id, CORES[id]->executing->ID);

                    break;
                }
                j++;
                kontagailua++;
                helbKont = helbKont + 4;
                kont2 = kont2 + 4;
                if(kont2 ==kontMax && sch == RR_SCH){
                    lagExecuted = 0;
                }
            }
            //Erlojuari seinalea bidali erlojua desblokeatzeko
            pthread_cond_signal(&cond1);
            //Erlojuaren seinaleari zain geratu
            pthread_cond_wait( &cond2, &mutex1 );
            atera = 1;
        }
        //Erlojuarekin sinkronizazioa mantentzeko
        if(!atera){
            //Erlojuari seinalea bidali erlojua desblokeatzeko
            pthread_cond_signal(&cond1);
            //Erlojuaren seinaleari zain geratu
            pthread_cond_wait( &cond2, &mutex1 );
        }
        
        //Round Robin bada egoera batzuetan datuak gorde beharko ditu
        if(CORES[id]->executing->STATE == EXECUTING && CORES[id]->egoera == EXECUTING && kontagailua>0 && lagExecuted == 0 && sch == RR_SCH){
            printf("Core %d: Datuak gordetzen\n", id);
            for(int z = 0; z<15; z++){
                CORES[id]->executing->registers[z] = registers[z];
            }
            lagExecuted = -1;
            kontagailua = 0;
            CORES[id]->executing->PC = helbKont;
            CORES[id]->executing->IR = j;
            CORES[id]->executing->STATE = READY;
            CORES[id]->executing->CPU_TIME = 1;
            CORES[id]->egoera = READY;
            if(CORES[id]->executing->ID == 2){
                registers[6] = 1;
            }
            executed = 1;
            
        }
        //Exekutatu bada bidali mezu bat schedulerraren tenporizadoreari
        if(lagExecuted == 1){
            executed = 1;
            lagExecuted = -1;
        }
        
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
        if (kont == SCH_FREC || executed){
            printf("(Tenp Scheduler): Sartu naiz. Schedulerrari seinalea bidaltzen...\n");
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
 * Libre dagoen core-aren indizea buetatzen du. Ez badago core librerik -1 bueltatuko du.
 */
int libre(){
    for(int i = 0; i<num_cores; i++){
        if(CORES[i]->egoera == READY)
            return i;
    }
    return -1;
}


/**
 * Schedulerraren funtzioa. Oraingoz schedulerraren tenporizadoreak bidali behar dion seinalearen zain dago. Hurrengo zatietan gauza geihago egingo ditu.
 */
void *scheduler(){

    printf("Schedulerra aktibatuta.\n");
        
    struct node *currentNode;

    currentNode = processQueue;
    int itxaron;

    int i = 0;
    //sch = 0;
    int indk;
    
    while(1){

        //Schedulerrak egin behar dituen gauzak hemen

        switch(sch){
            case FIFO_SCH:
                // Fifo exekutatu
                //Core librearen indizea lortu
                indk = libre();
                pthread_mutex_lock( &mutex2 );
                //Core librerik baldin badago
                if(indk!=-1){
                    //Aldatu egoera exekutatzeko prozesu librerik badago
                    if (currentNode->next != currentNode)
                    {   
                        currentNode = currentNode->next;
                        if(currentNode->data->ID == 0){
                            currentNode = currentNode->next;
                        }
                        if(currentNode->data->STATE == READY){
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            currentNode->data->STATE = EXECUTING;
                            CORES[indk]->executing = currentNode->data;
                            CORES[indk]->unekoNodo = currentNode;
                            CORES[indk]->egoera = EXECUTING;
                        }else{
                            itxaron = 1;
                        }
                    }else{
                        CORES[indk]->executing->ID = 0;
                        CORES[indk]->egoera = READY;
                        itxaron = 1;
                    }
                }
                pthread_mutex_unlock( &mutex2 );
                //Itxaron schedulerraren tenporizadoreari baldin eta core librerik ez badago edo prozesu berririk ez badago
                if(libre() == -1 || itxaron){
                    sem_wait(&sem_scheduler);
                    itxaron = 0;
                }
                printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");
                break;
            case RR_SCH:
                // RR exekutatu
                //Core librearen indizea lortu
                indk = libre();
                pthread_mutex_lock( &mutex2 );
                //Core librerik baldin badago
                if(indk!=-1 && CORES[indk]->egoera != EXECUTING){
                    //Aldatu egoera exekutatzeko prozesu librerik badago
                    if (currentNode->next != currentNode)
                    {   
                        currentNode = currentNode->next;
                        if(currentNode->data->ID == 0){
                            currentNode = currentNode->next;
                        }
                        if(currentNode->data->STATE == READY){
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            printf("Core %d: %d prozesua esleitu zait\n", indk, currentNode->data->ID);
                            currentNode->data->STATE = EXECUTING;
                            CORES[indk]->executing = currentNode->data;
                            CORES[indk]->unekoNodo = currentNode;
                            CORES[indk]->egoera = EXECUTING;
                        }else{
                            itxaron = 1;
                        }
                    }else{
                        CORES[indk]->executing->ID = 0;
                        CORES[indk]->egoera = READY;
                        itxaron = 1;
                    }
                }
                
                pthread_mutex_unlock( &mutex2 );
                //Itxaron schedulerraren tenporizadoreari baldin eta core librerik ez badago edo prozesu berririk ez badago
                if(libre() == -1 || itxaron){
                    sem_wait(&sem_scheduler);
                    itxaron = 0;
                }
                
                printf("(Scheduler): Seinalea jasota. Nire gauzak egiteko prest...\n");
                break;
            default:
                
               printf("Berriro saiatu\n");
        }
        
    }
}


/**
 * Prozesuak memoria fisikoan kargatzen dituen funtzioa.
*/
void loader(){
    
    printf("Loader funtzioa aktibatuta.\n");

    FILE *fp;
    char * filename = (char * )malloc(11*sizeof( char));
    char * filename2 = (char * )malloc(11*sizeof( char));
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char* ptr;
    int textLogAddress;
    int dataLogAddress;
    int helbLog;
    int i;
    int k;
    int progZenb;
    int textHelbFis;
    int dataHelbFis;
    progZenb = 0;
    printf("proba1\n");
    filename = "prog000.elf";
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    i = 0;
    k = PHYSICAL_MEMORY_SIZE-KERNEL_SIZE;
    //Fitxategiak ireki daitezkeen bitartean
    while( fp != NULL){
        

        printf("%s\n", filename);
        helbLog = 0x0;
        
        

        //Irakurri bi lehenengo lineak eta lortu testu eta dataren helbide logikoak
        
        read = getline(&line, &len, fp);
        ptr = strtok(line, " ");
        ptr = strtok(NULL, " ");

        textLogAddress = (int)strtol(ptr, NULL, 16);
        
        printf("Text addr: %X\n", textLogAddress);

        read = getline(&line, &len, fp);
        ptr = strtok(line, " ");
        ptr = strtok(NULL, " ");

        dataLogAddress = (int)strtol(ptr, NULL, 16);

        printf("Data addr: %X\n", dataLogAddress);
        //Aginduen helbide fisikoa gorde
        textHelbFis = i;
        while ((getline(&line, &len, fp)) != -1) {
            //read = getline(&line, &len, fp);
            printf("%d\n", i);
            PHYSICAL_MEMORY[i] = (int)strtol(line, NULL, 16);
            printf("Orri taula helb: %X\n", helbLog);
            if(helbLog == dataLogAddress){
                //Datuen helbide fisikoa gorde
                printf("%d\n",(__int32_t)PHYSICAL_MEMORY[i]);
                dataHelbFis = i;
            }
            
            helbLog = helbLog + 4;
            i++;
        }

        
        fclose(fp);
        //Kernel eremuan gorde prozesuaren orri taula
        PHYSICAL_MEMORY[k] = textHelbFis;
        k++;
        PHYSICAL_MEMORY[k] = dataHelbFis;
        k++;

        //Prozesu berria sortu
        printf("kaixo3\n");
        //Prozesua sortu eta hasieraketak egin
        struct pcb *newPcb = (struct pcb*)malloc(sizeof(struct pcb));  //Memoria alokatu
        
        newPcb->ID = prozesu_id;    //Prozesuaren identifikadorea finkatu
        newPcb->STATE = READY;      //Prozesuaren hasierako egoera finkatu
        newPcb->CPU_TIME = 0;
        newPcb->IR = 0;  
        newPcb->PC = 0x0;
        struct mm *newMM = (struct mm*)malloc(sizeof(struct mm));
        newMM->datuak = dataLogAddress;
        newMM->code = textLogAddress;
        newMM->pgb = k-2; 
        newPcb->MEMORY_MANAGEMENT = newMM;
        prozesu_kop++;
        prozesu_id++;                                                  //Prozesu kopurua handitu
        pthread_mutex_lock( &mutex2 );
        
        insertFirst(&processQueue, newPcb);                            //Prozesua ilaran sartu

        pthread_mutex_unlock( &mutex2 );
        
        progZenb++;

        sprintf(filename2, "prog%.3d.elf", progZenb);
        filename = filename2;
        fp = fopen(filename, "r");
        
    }
    printProcessQueue(processQueue);
}

int main(int argc, char *argv[]) {

    
    pthread_t tenp_pr_sor, tenp_scheduler, erlj, pr_sor, tscheduler;

    //Memoria fisikoa sortu eta alokatu
    for(int i = 0; i<PHYSICAL_MEMORY_SIZE; i++){
        PHYSICAL_MEMORY[i] = malloc(4); //Malloc bakoitzak 4 byte
    }

    //Fifo edo RoundRobin hautatzeko
    if(argc>1 && strcmp(argv[1], "fifo")==0){
        sch = 0;
    }else{
        sch = 1;
    }

    //Prozesu nulua sortzen
    struct pcb *prozesu_nulua = (struct pcb*)malloc(sizeof(struct pcb));
    prozesu_nulua->ID = 0;
    prozesu_nulua->STATE = READY;
    prozesu_nulua->CPU_TIME = 0;
    prozesu_nulua->beharrezko_denbora = 100000000;

    //Prozesuen ilara zirkularra sortzen sortu
    processQueue = createCircularQueue();

    insertFirst(&processQueue, prozesu_nulua);

    //Programak kargatu memorian
    loader();
    
    //Prozesu sortzailea eta bere tenporizadorea sinkronizatuta mantenduko duen semaforoa hasieratu
    sem_init(&sem_pr_sor, 0, 0);

    //Schedulerra eta bere tenporizadorea sinkronizatuta mantenduko duen semaforoa hasieratu
    sem_init(&sem_scheduler, 0, 0);
    if(argc>2){
        num_cores = atoi(argv[2]);
    }
    else{
        num_cores = 1;
    }
    
    //Cores listaren tamaina definitzen
    CORES[num_cores];

    //Core-ak sortu eta Cores listan sartu
    for(int i = 0; i<num_cores; i++){
        CORES[i] = (struct core*)malloc(sizeof(struct core));
        CORES[i]->executing = (struct pcb*)malloc(sizeof(struct pcb));
        CORES[i]->executing->ID = 0;
        CORES[i]->ID = i;
        CORES[i]->IR = 0;
        CORES[i]->PC = 0;
        CORES[i]->sartu = 1;
        CORES[i]->egoera = READY;
        CORES[i]->unekoNodo = (struct node*)malloc(sizeof(struct node));
        CORES[i]->registers[15];
    }
    
    //Erlojua sortu
    pthread_create( &erlj, NULL, erlojua, NULL);

    pthread_t cores[num_cores];
    //Core-en hariak sortu
    for (int i = 0; i<num_cores; i++){
        pthread_create( &cores[i], NULL, core, (void *)&i);
        tenp_kop++;
    }

    //Schedulerraren tenporizadorea sortu
    pthread_create( &tenp_scheduler, NULL, tenporizadorea_scheduler, NULL);
    tenp_kop++;

    //Schedulerra sortu
    pthread_create( &tscheduler, NULL, scheduler, NULL);

    pthread_join(erlj, NULL);
}