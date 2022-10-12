#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <regex.h> 
                        // IMPLEMENTARE USO MEMORIA
#include <sys/types.h>
#include <sys/dir.h>

#include <sys/stat.h>
#include <fcntl.h>

// provvisorie 
#include <stdint.h>
#include "sys/sysinfo.h"

// implementa struttura con info per ogni  processo 
// struttura per dati sistema 
typedef struct cpu_info {    // cpu info
    int     cpu_usage;
    int     mem_usage;
    int     mem_free;
    int     cpu_usage_after;
    int     mem_usage_after;
} cpu_info_t;
// struttura per dati processo ( e calcolo cpu )
typedef struct proc_info_new {
    int     PID;
    char stato;
    char nome[30];
    unsigned long int     time_u;
    unsigned long int     time_k;
    unsigned long int     time_u_after;
    unsigned long int     time_k_after;
    int time_cu;
    int time_ck;
    int time_cu_after;
    int time_ck_after;
} proc_info_new_t;
// struttura info processo per cpu/memory percentuale
typedef struct proc_stat{
    int     PID;
    char stato;
    char nome[30];
    float cpu_usage_u;
    float cpu_usage_k;
    float memory_usage;
}proc_stat_t;




#define QUIT_COMMAND  "QUIT"
void get_mem_proc(float* qui);                   // prende memoria fisica e virtuale singolo processo  ( aggiungi char* per dargli pid in input)

void get_mem_sistema();

int parseLine(char* line);

char** alloca_m(int n);  // aloca matrice s

void printa(void* struttura);

int popola_ris(char** m,int n,struct dirent** files_1); // seleziona solo pid da /proc

void get_info_proc(char* pid,void* struttura,int t);  // prende info singolo processo

void get_percent_one(char* pid_da_arr,void* struttura_cpu,void* struttura_proc , void* stat_struct);  

void get_info_tot(void* info_sis, int t);  // orende innfo sistema (da finire con memoria)

int main(int argc,char* argv[]){
    float val=0;
    float* p_val=&val;
    

    // ORGANIZZO VARIABILI
    struct dirent **files;
    int r;
    char** file_in_proc;
    char *quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    //struct proc_info_

    //chiamo fnz per val iniziali


    //printf("lunghezza quit command == %ld\n",quit_command_len);

    char*arg1=malloc(20);
    char*arg2=malloc(20);
    if (argc > 1) arg1 = argv[1];
	if (argc > 2) arg2 = argv[2];


    while(1){

        printf("sono nel ciclo\n");
      


        char* richiesta=(char*)malloc(20);
        memset((void*)richiesta,0,20);
        richiesta=fgets(richiesta, 20, stdin);  // prendo input

        //printf("primo carattere == %c\n",richiesta[0]);

        int msg_len = strlen(richiesta);
        richiesta[--msg_len]='\0';         //se voglio togliere \n
        //printf("lunghezza == %d\n",msg_len);



        //while ( msg_len != quit_command_len && memcmp(richiesta, quit_command, quit_command_len)!=0) continue; non so perchè ma da problemi(senza funge)

        if(memcmp(richiesta, quit_command, quit_command_len)==0){printf("qui");break;}
    



       
   
        //printf("eseguo comando == %s\n",richiesta);
        
        //----------------------------------------------------------------------------------------
        // getcwd per la directory corrente

      
        
        int n = scandir("/proc", &files, NULL, alphasort); // metto risultato scandir nella struct files

        if (n < 0) {
            printf("errore richiesta file inp proc");
            exit(EXIT_FAILURE);
        }
    

        file_in_proc=alloca_m(n);        // alloco matrice per nomi

      
        int quanti=popola_ris(file_in_proc,n,files);       // filtro risultati

        /*  check file selezionati
        printf("stampo file salvati in proc (quelli numerici)\n");
        
        for (int j=0;j<quanti;j++){

            printf("file == > %s\n", file_in_proc[j]);
        }
        printf("FINE STAMPA\n");*/


       // inizializzo strutture necessarie                                                      CPU STAT
        cpu_info_t* struttura = malloc(sizeof(cpu_info_t));
        proc_info_new_t* info_proc = malloc(sizeof(proc_info_new_t));
        proc_stat_t* riempi=malloc(sizeof(proc_stat_t));

        //char* analizza=file_in_proc[0];

        //get_percent_one("4758",(void*)struttura,(void*)info_proc,(void*)riempi);

        //printa((void*)riempi);

        // raccolgo e printo percentuali CPU usage

        //  --------------------------------------------------------------------------------------------
         printf("\nRICHIEDO DATI MEM PROCESSO \n");
        get_mem_proc(p_val);                             // MEORIA SINGOLO PROCESSO------------------FUNZIONA

        //get_mem_sistema();


           // I VALORIPRESI SONO QUELLI DELLA RAM FISICA , vm rss , vm size della virtuale capire come ottenere valore %MEM che vedi in top

           //prendi vm_rss (ram proc) dividi per 1024 poi dividi per (ram totale/1024)
        printf("RAM processo == %f\n",*p_val);

        *p_val=*p_val/1024;

        printf("RAM processo == %f\n",*p_val);


        float per_mem=(float)(*p_val)/6839.2;

        printf("percentuale uso ram == %f\n\n",per_mem*100);

        printf("USCITO DA FUNZIONE");




        

     





        // free 
        for (int i = 0; i < n; i++) {
            free(files[i]);
        }
        free(files);

        printf("\nDEALLOCATO\n");




    }


    printf("entrato comando quit \n");
    fflush(stdout);
    
    return 0;

}
////////////////////////-----------------------------------------------------------------------------

void get_percent_one(char* pid_da_arr,void* struttura_cpu,void* struttura_proc , void* stat_struct){

    

        get_info_tot((void*)struttura_cpu,1);

        get_info_proc(pid_da_arr,(void*)struttura_proc,1);  // prima della sleep

        sleep(3);

        get_info_tot((void*)struttura_cpu,0);

        get_info_proc(pid_da_arr,(void*)struttura_proc,0);


        proc_info_new_t* p=(proc_info_new_t*)struttura_proc;
        cpu_info_t* p1=(cpu_info_t*)struttura_cpu;
        proc_stat_t* stat=(proc_stat_t*)stat_struct;


       /* printf("RESOCONTO SINGOLO PROCESSO %s \n",p->nome);

        /*printf("\ntrovati dati SISTEMA:\ncpu tot == %d\nmem_usage == %d\n",p1->cpu_usage,p1->mem_usage);

        printf("trovati dati SISTEMA after :\ncpu tot == %d\nmem_usage == %d\n",p1->cpu_usage_after,p1->mem_usage);

        printf("\nraccolti dati PROCESSO: \n tempo_user == %ld\n tempo_k == %ld\n",p->time_u,p->time_k);

        printf("raccolti dati PROCESSO after: \n tempo_user_a == %ld\n tempo_k_a == %ld\n",p->time_u_after,p->time_k_after);
*/

        int ris=p->time_u_after-p->time_u;
        int ris_cu=(int)(p->time_cu_after-p->time_cu);
        printf("DIFFERENZA Utime_processo == %d\n",ris);
        int ris_2=p->time_k_after-p->time_k;
        int ris_ck=(int)(p->time_ck_after-p->time_ck);
        //printf("DIFFERENZA Ktime_processo == %d\n",ris_2);
        int ris_1=p1->cpu_usage_after-p1->cpu_usage;
        printf("DIFFERENZA time CPU == %d\n",ris_1);
        
        float perc=(float)(ris+ris_cu)/(float)ris_1;
        float perc1=(float)(ris_2+ris_ck)/(float)ris_1;

        printf("prcentuale== %f  \n",(perc+perc1)*100);

        stat->cpu_usage_u=perc*100;
        stat->cpu_usage_k=perc1*100;

        //stat->cpu_usage_u=perc;
        //stat->cpu_usage_k=perc1;
      
        stat->PID=p->PID;
        strcpy(stat->nome,p->nome);
        stat->stato=p->stato;
     

}




void get_info_proc(char* pid,void* struttura, int t){

    proc_info_new_t* dati=(proc_info_new_t*)struttura;
    //memset((void*)dati,0,sizeof(dati));
    


    long unsigned int valori_lu[6];
    int valori_i[8];
    unsigned val_u;
    char* nome_p=malloc(30);
    char stato;
    memset((void*)valori_lu,0,6);
    memset((void*)valori_i,0,8);
    


    // /proc/PID/stat
    char* percorso=malloc(20);
    memset((void*)percorso,0,20);
    const char* pid_1=pid;

    strcat(percorso, "/proc/");
    strcat(percorso, pid_1);
    strcat(percorso, "/stat");
    if(t!=0){
    printf("percorso cercato== %s\n",percorso);
    }
    FILE* fd=fopen(percorso,"r");
    if(fd==NULL){
        printf("errore apertura il processonon esiste\n");
        exit(EXIT_FAILURE);
    }
    // 14 ,15 u k 16 17 figli
    char line[128];
    //fgets(line, 128, fd);
    //printf("stringa info processo \n %s\n",line);


    fscanf(fd,"%d %[^)]) %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %d %d",&valori_i[0],nome_p,&stato,&valori_i[1],&valori_i[2],&valori_i[3],&valori_i[4],&valori_i[5]
              ,&val_u,&valori_lu[0],&valori_lu[1],&valori_lu[2],&valori_lu[3],&valori_lu[4],&valori_lu[5],&valori_i[6]
              ,&valori_i[7] );

    fclose(fd);
    //printf("vediamo %d %s stato %c %d %d %d %d %d %u %lu %lu %lu %lu \n questo %lu %lu %d %d \n",valori_i[0],nome_p,stato,valori_i[1],valori_i[2],valori_i[3],valori_i[4],valori_i[5]
    //          ,val_u,valori_lu[0],valori_lu[1],valori_lu[2],valori_lu[3],valori_lu[4],valori_lu[5],valori_i[6],valori_i[7]);
    printf("vediamo %d %s stato %c\n questo %lu %lu %d %d \n",valori_i[0],nome_p,stato,valori_lu[4],valori_lu[5],valori_i[6],valori_i[7]);
    
    dati->PID=valori_i[0];
    dati->stato=stato;
    //dati->nome=nome_p;
    //printf("lunghezza nome == %ld\n",strlen(nome_p));
    strcpy(dati->nome,nome_p);
    dati->time_cu=valori_i[6];
    dati->time_ck=valori_i[7];

    if(t!=0){
        dati->time_u=valori_lu[4];
        dati->time_k=valori_lu[5];
        dati->time_cu=valori_i[6];
        dati->time_ck=valori_i[7];
    }
    else{
        dati->time_u_after=valori_lu[4];
        dati->time_k_after=valori_lu[5];
        dati->time_cu_after=valori_i[6];
        dati->time_ck_after=valori_i[7];
    }
   

    return;
}

void get_info_tot(void* info_sis, int t){

    cpu_info_t* dati=(cpu_info_t*)info_sis;

    int v1=0,v2=0,v3=0,v4=0,v5=0,v6=0,v7=0;
    char* n=malloc(5);
    FILE* fd=fopen("/proc/stat","r");
    if(fd==NULL){
        printf("errore apertura\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fd,"%s %d %d %d %d %d %d %d \n",n,&v1,&v2,&v3,&v4,&v5,&v6,&v7);
    int sum=v1+v2+v3+v4+v5+v6+v7;   //somma cpu giusta
    //printf("ris == %d\n",sum);
    //printf("aperto\n");

    fclose(fd);

    int mem_tot=0,mem_free=0;
    char* c1=(char*)malloc(20);
    char *c3=(char*)malloc(20);
    char* c2=malloc(4);
    FILE* fd_1=fopen("/proc/meminfo","r");
    if(fd_1==NULL){
        printf("errore apertura\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fd_1,"%s %d %s %s %d",c1,&mem_tot,c2,c3,&mem_free);    // valori giusti
    //printf("memoria totale == %d\nmemoria libera == %d\n",mem_tot,mem_free);

    if(t!=0){
        dati->cpu_usage=sum;
        dati->mem_usage=mem_tot;
        }
    else{
        dati->cpu_usage_after=sum;
        dati->mem_usage_after=mem_tot;
    }
    dati->mem_free=mem_free;
    

    fclose(fd_1);

    return;
}


char** alloca_m(int n){
    char** m;
    m = malloc(n * sizeof(char *));
    for (int r = 0; r < n; r++){           
        *(m + r) = malloc(20 * sizeof(char)); // 20 abbastanza arbitrario (dovresti aggiustare) (lunghezza pid)
    }
    return m;

}

int popola_ris(char** m,int n, struct dirent** files_1){
    int k=0;

    for (int i = 0; i < n; i++) {

            /* associo risultato a struttura  ( guarda altre voci )*/

            struct dirent *file_dir = files_1[i];

            // fai funzione apposita magari dove settare regex
            regex_t regex;
            int r = regcomp(&regex, "[0-9]", 0);
            if (r) {
                fprintf(stderr, "Could not compile regex\n");
                exit(1);
            }


            r = regexec(&regex, file_dir->d_name, 0, NULL, 0);

                if( !r ){
                        //puts("Match");
                        m[k] = file_dir->d_name;
                        //printf("nella fnz %s\n", m[k]);
                        k++;
                }
                else if( r == REG_NOMATCH ){
                        //puts("No match");
                }
                else{
                        
                    fprintf(stderr, "Regex match failed\n");
                    exit(1);
                }

            regfree(&regex);

        }

       return k;

}

void printa(void* struttura){

    proc_stat_t* p=(proc_stat_t*)struttura;

    printf("\n%d     %c      %s       %f       %f\n",p->PID,p->stato,p->nome,p->cpu_usage_u,p->cpu_usage_k);

    free(p);


}

/*
typedef struct proc_stat{
    int     PID;
    char stato;
    char nome[30];
    float cpu_usage_u;
    float cpu_usage_k;
    float memory_usage;
}proc_stat_t;

*/
void get_mem_proc(float* qui){ 

    /*strcat(percorso, "/proc/");
    strcat(percorso, pid_1);
    strcat(percorso, "/stat");*/

    FILE* file = fopen("/proc/2222/status", "r");
    if(file==NULL){
        printf("errore apertura file proc_mem\n");
        exit(EXIT_FAILURE);
    }
    char line[128];
    int res=0;
    FILE* file_1 = fopen("/proc/7696/status", "r");
    if(file==NULL){
        printf("errore apertura file proc_mem\n");
        exit(EXIT_FAILURE);
    }
    
    char line_1[128];
    int res_1=0;

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){        // fisica
            res = parseLine(line);
            break;
        }
    }
       while (fgets(line_1, 128, file_1) != NULL){
        if (strncmp(line_1, "VmSize:", 7) == 0){     // virtuale
            res_1 = parseLine(line_1);
            break;
        }
    }
    fclose(file);
    fclose(file_1);
    printf("Memoria fisica proc == %d\n",res);
    *qui=res;
    printf("Memoria virtuale proc  == %d\n",res_1);
    

    return;
}
int parseLine(char* line){   
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

void get_mem_sistema(){
    struct sysinfo memInfo;

    sysinfo (&memInfo);

    int32_t p=0;
    int32_t after=0;
    p=(uint64_t)memInfo.totalram;

    after = ((uint64_t)memInfo.totalram * memInfo.mem_unit)/1024;          // giusto valore totale (mem total (RAM))


    printf("controllo valori %d mem unit == %d\n",p,memInfo.mem_unit);

    printf("VEDIAMO == %d\n",after);



}