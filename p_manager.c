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


#define QUIT_COMMAND  "QUIT"

char** alloca_m(int n);
int popola_ris(char** m,int n,struct dirent** files_1);

//void get_info_proc(char* pid,struct proc_info_t* struttura);

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


        proc_info_t* struttura = malloc(sizeof(proc_info_t));

        char* richiesta=(char*)malloc(20);
        richiesta=fgets(richiesta, 20, stdin);  // prendo input

        //printf("primo carattere == %c\n",richiesta[0]);

        int msg_len = strlen(richiesta);
        richiesta[--msg_len]='\0';  //se voglio togliere \n
        printf("lunghezza == %d\n",msg_len);



        while ( msg_len != quit_command_len && memcmp(richiesta, quit_command, quit_command_len)!=0) continue;

        if(memcmp(richiesta, quit_command, quit_command_len)==0)break;
    



       
   
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

        // prendo info base

        

        get_info_tot((void*)struttura);

        printf("trovati dati:\ncpu tot == %d\nmem_usage == %d\nmem_free == %d\n",struttura->cpu_usage,struttura->mem_usage,struttura->mem_free);


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

        printf("DEALLOCATO\n");

        //--------------------------------------------prendo info-------------------------------------------------------


    }




    printf("entrato comando quit \n");
    fflush(stdout);
    


}





void get_info_tot(void* info_sis){

    proc_info_t* dati=(proc_info_t*)info_sis;

    int val1=0,val2=0,val3=0,val4=0,val5=0,val6=0,val7=0;
    char* n=malloc(5);
    FILE* fd=fopen("/proc/stat","r");
    if(fd==NULL){
        printf("errore apertura\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fd,"%s %d %d %d %d %d %d %d \n",n,&val1,&val2,&val3,&val4,&val5,&val6,&val7);
    int sum=val1+val2+val3+val4+val5+val6+val7;   //somma cpu giusta
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
