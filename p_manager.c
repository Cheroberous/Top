#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <regex.h> 

#include <sys/types.h>
#include <sys/dir.h>

#include <sys/stat.h>
#include <fcntl.h>

// implementa struttura con info per ogni  processo 
typedef struct proc_info_s {
    int     PID;
    int     cpu_usage;
    int     mem_usage;
    int     mem_free;
} proc_info_t;

typedef struct proc_info_new {
    int     PID;
    char stato;
    char nome[20];
    unsigned long int     time_u;
    unsigned long int     time_k;
    int time_cu;
    int time_ck;
} proc_info_new_t;




#define QUIT_COMMAND  "QUIT"

char** alloca_m(int n);
int popola_ris(char** m,int n,struct dirent** files_1);

void get_info_proc(char* pid,void* struttura);

void get_info_tot(void* info_sis);

int main(int argc,char* argv[]){

    // ORGANIZZO VARIABILI
    struct dirent **files;
    int r;
    char** file_in_proc;
    char *quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    //struct proc_info_

    //chiamo fnz per val iniziali


    printf("lunghezza quit command == %ld\n",quit_command_len);

    char*arg1=malloc(20);
    char*arg2=malloc(20);
    if (argc > 1) arg1 = argv[1];
	if (argc > 2) arg2 = argv[2];


    while(1){

        printf("sono nel ciclo\n");
        proc_info_t* struttura = malloc(sizeof(proc_info_t));
        proc_info_new_t* info_proc = malloc(sizeof(proc_info_new_t));


        char* richiesta=(char*)malloc(20);
        richiesta=fgets(richiesta, 20, stdin);  // prendo input

        //printf("primo carattere == %c\n",richiesta[0]);

        int msg_len = strlen(richiesta);
        richiesta[--msg_len]='\0';  //se voglio togliere \n
        printf("lunghezza == %d\n",msg_len);



        while ( msg_len != quit_command_len && memcmp(richiesta, quit_command, quit_command_len)!=0) continue;

        if(memcmp(richiesta, quit_command, quit_command_len)==0){printf("qui");break;
    



       
   
        printf("eseguo comando == %s\n",richiesta);
        
        //----------------------------------------------------------------------------------------
        // getcwd per la directory corrente

      
        
        int n = scandir("/proc", &files, NULL, alphasort); // metto risultato scandir nella struct files

        if (n < 0) {
            printf("errore richiesta file inp proc");
            exit(EXIT_FAILURE);
        }
 

        // alloco matrice per nomi

        file_in_proc=alloca_m(n);      // n

        // filtro risultati

        int quanti=popola_ris(file_in_proc,n,files);

        // prendo info base   SISTEMA

        get_info_tot((void*)struttura);

        printf("trovati dati:\ncpu tot == %d\nmem_usage == %d\nmem_free == %d\n",struttura->cpu_usage,struttura->mem_usage,struttura->mem_free);

        get_info_proc("3758",(void*)info_proc);


        printf("USCITO DA FUNZIONE");

        //printf("valori raccolti: \n PID == %d \ntempo user mode == %lu \n tempo kernel mode %lu \n_",info_proc->PID,info_proc->time_u,info_proc->time_k);

        /*  check file selezionati
        printf("stampo file salvati in proc (quelli numerici)\n");
        
        for (int j=0;j<quanti;j++){

            printf("file == > %s\n", file_in_proc[j]);
        }
        printf("FINE STAMPA\n");*/





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




void get_info_proc(char* pid,void* struttura){

    proc_info_new_t* dati=(proc_info_new_t*)struttura;
    //memset((void*)dati,0,sizeof(dati));
    


    long unsigned int valori_lu[6];
    int valori_i[8];
    unsigned val_u;
    char* nome_p=malloc(20);
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
    printf("percorso cercato== %s\n",percorso);

    FILE* fd=fopen(percorso,"r");
    if(fd==NULL){
        printf("errore apertura\n");
        exit(EXIT_FAILURE);
    }
    // 14 ,15 u k 16 17 figli


    fscanf(fd,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %d %d \n",&valori_i[0],nome_p,&stato,&valori_i[1],&valori_i[2],&valori_i[3],&valori_i[4],&valori_i[5]
              ,&val_u,&valori_lu[0],&valori_lu[1],&valori_lu[2],&valori_lu[3],&valori_lu[4],&valori_lu[5],&valori_i[6]
              ,&valori_i[7] );





    fclose(fd);
    printf("vediamo %d %s %c %d %d %d %d %d %u %lu %lu %lu %lu \n questo %lu %lu %d %d \n",valori_i[0],nome_p,stato,valori_i[1],valori_i[2],valori_i[3],valori_i[4],valori_i[5]
              ,val_u,valori_lu[0],valori_lu[1],valori_lu[2],valori_lu[3],valori_lu[4],valori_lu[5],valori_i[6],valori_i[7]);
    
    dati->PID=valori_i[0];
    //dati->nome=nome_p;
    //strcpy(dati->nome,nome_p);
  

    dati->time_u=valori_lu[4];
    dati->time_k=valori_lu[5];
    dati->time_cu=valori_i[6];
    dati->time_ck=valori_i[7];

    return;
}

void get_info_tot(void* info_sis){

    proc_info_t* dati=(proc_info_t*)info_sis;

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

    dati->cpu_usage=sum;
    dati->mem_usage=mem_tot;
    dati->mem_free=mem_free;
    dati->PID=0;

    fclose(fd_1);

    return;
}


char** alloca_m(int n){
    char** m;
    m = malloc(n * sizeof(char *));
    for (int r = 0; r < n; r++){
            
        *(m + r) = malloc(20 * sizeof(char)); // 20 abbastanza arbitrario (dovresti aggiustare)
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
