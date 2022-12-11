#include "datu_egiturak.h"
#include "circular_queue.h"

int PHYSICAL_MEMORY[100];

/**
 * Prozesu sortzailearen tenporizadoreak seinale bat bidaltzean prozesu berri bat sortu eta prozesuen listan sartuko du.
 */
void loader(){
    
    printf("Prozesu sortzailea aktibatuta.\n");

    FILE *fp;
    char filename[128] = "prog000.elf";
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char* ptr;
    int textLogAddress;
    int dataLogAddress;
    int helbLog;
    int i;
    int j = 0;
    __int32_t a;
    for(int i = 0; i<100; i++){
        PHYSICAL_MEMORY[i] = malloc(4);
    }
    printf("proba1\n");
    while(j<1){
        i = 0;
        j++;
        //Prozesu sortzailearen tenporizadorearen semaforoari zain geratu
        //sem_wait(&sem_pr_sor);
        helbLog = 0x0;
        fp = fopen(filename, "r");
        
        if (fp == NULL)
            exit(EXIT_FAILURE);

        //Irakurri bi lehenengo lineak eta lortu testu eta dataren helbide logikoak
        
        read = getline(&line, &len, fp);
        ptr = strtok(line, " ");
        ptr = strtok(NULL, " ");

        textLogAddress = (int)strtol(ptr, NULL, 16);
        printf("hola\n");
        printf("Text addr: %X\n", textLogAddress);

        read = getline(&line, &len, fp);
        ptr = strtok(line, " ");
        ptr = strtok(NULL, " ");

        dataLogAddress = (int)strtol(ptr, NULL, 16);

        printf("Data addr: %X\n", dataLogAddress);

        while ((getline(&line, &len, fp)) != -1) {
            //read = getline(&line, &len, fp);
            PHYSICAL_MEMORY[i] = (int)strtol(line, NULL, 16);
            printf("Orri taula helb: %X\n", helbLog);
            if(helbLog >= dataLogAddress){
                //printf("exit\n");
                printf("%d\n",(__int32_t)PHYSICAL_MEMORY[i]);
            }
            printf("EL resto: %X\n", PHYSICAL_MEMORY[i]);
            helbLog = helbLog + 4;
            i++;
        }

        fclose(fp);
        //if (line)
        //    free(line);
        //exit(EXIT_SUCCESS);

        printf("(Prozesu sortzailea): Seinalea jasota. Nire gauzak egiteko prest...\n");

        //Prozesu berria sortu
        /*
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
        
        printProcessQueue(processQueue);*/
        
    }
}

int main(int argc, char *argv[]) {
    char hex[] = "17000050";                          // here is the hex string
    int num = (int)strtol(hex, NULL, 16);   
    printf("%X\n", num); 
    printf("%ld\n", sizeof(num));
    printf("%X\n", num+4); 

    loader();
}