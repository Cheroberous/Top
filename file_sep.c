#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>             
#include <string.h>
#include <unistd.h>
#include <regex.h>                      // kill -9 -1 Kill all processes you can kill.
#include <signal.h>                      
#include <sys/types.h>
#include <sys/dir.h>                    // se ho tempo metto kill all
#include <pthread.h>                   // lista fd processo
#include <sys/stat.h>                  // DEVO FARE LA FREE DELLE STRUTTURE + CHECK valgrind
#include <fcntl.h>
// provvisorie 
#include <stdint.h>
#include "sys/sysinfo.h"
#include "fnz.h"




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
    float memory_usage;              // potrei aggiungere un campo qui per la vmsize e semplificare il print
}proc_stat_t;

typedef struct thread_args_s {
    struct proc_stat* p_s;
    struct proc_info_new* p_i_n;
    struct cpu_info* c_i;
    char* pid_proc;
    float valore_ram;

} thread_args_t;







int main(int argc,char* argv[]){
    float val=0;
    float* p_val=&val;
    

    // ORGANIZZO VARIABILI
    struct dirent **files;   // struttura per ls
    struct dirent** lista_fd;  // struttura fd singolo proc
    int r;
    int r2;
    char** file_in_proc;                         // array con lista processi
    char *quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command); 
    int stoppa_len=strlen(STOPPA);
    int resume_len=strlen(RIPRENDI);
    int ter_len=strlen(TERMINA);
    int kill_len=strlen(KILLA);   
    char*cosa=malloc(10);
    char* richiesta=(char*)malloc(20);
    int quale_proc=0;  
 
    char* process=(char*)malloc(MAX_PID);          
    float fis;
    float vir;
    
     // richiedo ram totale pc e info sistema
    get_dettagli_cpu();
    int r1=get_mem_sistema();
    float Ram=(float)(r1/1024);
    printf("Ram             : %0.2f Mb\n\n",Ram);


    printf("- Il programma lista i processi attivi con stato , percentuali ram e cpu consumate; per mandare signal ai processi è possibile usare\n- il formato stop/resume/termina/kill + pid, per avere i fd di un processo usare filed + pid\n");
    



   



   
    
   // LOOP PRINCIPALE

    while(1){
        memset(cosa,0,strlen(cosa));
        memset((void*)richiesta,0,20); // il for inizializza a 0 le altre strutture


        printf("inserisci comando\n");
      
       // PRENDO E VERIFICO INPUT

        
        
        richiesta=fgets(richiesta, 20, stdin);  // prendo input

        int msg_len = strlen(richiesta);
        richiesta[--msg_len]='\0';         //se voglio togliere \n

      

        if(memcmp(richiesta, quit_command, quit_command_len)==0){printf("qui");break;}   // Controllo se l'utente vuole uscire
    

        printf("eseguo comando == %s\n",richiesta);

        // controllo che tipo di richhiesta ho avuto

        sscanf(richiesta,"%s %d",cosa,&quale_proc);

        
    

        if(quale_proc!=0){        
            //terminate kill suspend resume
            //SIGTERM 15 
            //SIGKILL 9
            //SIGSTOP 19
            //SIGCONT 18
            /*'D' = UNINTERRUPTABLE_SLEEP
            'R' = RUNNING & RUNNABLE
            'S' = INTERRRUPTABLE_SLEEP
            'T' = STOPPED
            'Z' = ZOMBIE*/    
            printf("comando == %s  processo == %d\n",cosa,quale_proc);
            char*conv_pid=malloc(MAX_PID);     
            sprintf(conv_pid, "%d", quale_proc);   
            r2=0;
            if(memcmp(richiesta,"filed",strlen("filed"))==0){
                    get_fd_programma(lista_fd,conv_pid);

            }

            if(memcmp(richiesta, STOPPA, stoppa_len)==0){
               r2=kill((pid_t)quale_proc,SIGSTOP);    // dovrebbe essere un long int
                if(r2==0)printf("messo in pausa processo\n");
                else{printf("processo non stoppabile o inesistente\n");}
            }
            if(memcmp(richiesta, RIPRENDI, resume_len)==0){
               r2=kill((pid_t)quale_proc,SIGCONT);   
                if(r2==0)printf("processo ristartato\n");
                else{printf("processo non ristartabile o inesistente\n");}
            }
            if(memcmp(richiesta, KILLA, kill_len)==0){
               r2=kill((pid_t)quale_proc,SIGKILL);   
                if(r2==0)printf("processo killato\n");
                else{printf("processo non killabile o inesistente\n");}
            }
            if(memcmp(richiesta, TERMINA, ter_len)==0){
               r2=kill((pid_t)quale_proc,SIGTERM);   
                if(r2==0)printf("processo terminato\n");
                else{printf("processo non terminabile o inesistente\n");}
            }
            free(conv_pid);

        }


        

        //SCANDISCO DIR PROC E POPOLO ARRAY CON PID

        else{
            //printf("dovrei far vedere processi\n");
        
        int n = scandir("/proc", &files, NULL, alphasort); // metto risultato scandir nella struct files

        if (n < 0) {
            printf("errore richiesta file in proc");
            exit(EXIT_FAILURE);
        }
    

        file_in_proc=alloca_m(n);        // alloco matrice per nomi

      
        int quanti=popola_ris(file_in_proc,n,files);       // filtro risultati       ---------------NUMERO PROCESSI




     
  

 

        //  --------------------------------------------------------------------------------------------  //

         printf("\nRICHIEDO DATI PROCESSI  \n");

        // dovrò ciclare su tutti quelli in "file in proc"

        // CICLO SUI TUTTI I PROCESSI
        cpu_info_t** lista_struttura=malloc(quanti*sizeof(cpu_info_t*));
        proc_info_new_t** lista_info_proc=malloc(quanti*sizeof(proc_info_new_t));
        proc_stat_t** lista_riempi=malloc(quanti*sizeof(proc_stat_t*));

        printf(" NOME                   PID               STATO                CPU            RAM                 VM ");

        for(int i=0;i<quanti;i++){                  // quanti 

        cpu_info_t* struttura = lista_struttura[i];
        struttura=malloc(sizeof(cpu_info_t));

        proc_info_new_t* info_proc = lista_info_proc[i];
        info_proc=malloc(sizeof(proc_info_new_t));

        proc_stat_t* riempi=lista_riempi[i];
        riempi=malloc(sizeof(proc_stat_t));

            memset((void*)process,0,MAX_PID);
            memset((void*)struttura,0,sizeof(cpu_info_t));
            memset((void*)info_proc,0,sizeof(proc_info_new_t));
            memset((void*)riempi,0,sizeof(proc_stat_t));

            pthread_t thread_handle;

            thread_args_t* args = malloc(sizeof(thread_args_t));
            args->pid_proc=file_in_proc[i];
            //args->pid_proc="9341";
            args->c_i=struttura;
            args->p_i_n=info_proc;
            args->p_s=riempi;
            args->valore_ram=Ram;

           
        
            if (pthread_create(&thread_handle, NULL, get_percent_one, args)) {
                printf("FATAL ERROR: cannot create thread \nExiting...\n");
                exit(1);
            }

            pthread_detach(thread_handle);     
           

            

         } 
          
        //for (int i = 0; i < quanti; i++) {
        //    free(file_in_proc[i]);
        //}
       
        //printf("\nDEALLOCATO\n");        // TESTALO*/

        } // parentesi per evitare di far partire calcolo proc
        

 

    }




    printf("entrato comando quit \n");
    fflush(stdout);
    
    return 0;

}
////////////////////////-----------------------------------------------------------------------------
//void* get_percent_one(char* pid_da_arr,void* struttura_cpu,void* struttura_proc , void* stat_struct);  // ricava percentuali (cpu) di uno
//
void* get_percent_one(void* arg_struct){

    thread_args_t* args = (thread_args_t*) arg_struct;

        cpu_info_t* struttura_cpu = args->c_i;
        proc_info_new_t* struttura_proc = args->p_i_n;
        proc_stat_t* stat_struct= args->p_s;

        memset(struttura_cpu,0,sizeof(cpu_info_t));
        memset(struttura_proc,0,sizeof(proc_info_new_t));
        memset(stat_struct,0,sizeof(proc_stat_t));

        char* pid_da_arr=malloc(10);
        pid_da_arr=args->pid_proc;

        float fis=0;
        float vir=0;

        
        get_mem_proc(&fis,&vir,pid_da_arr,args->valore_ram);

        
        if(fis<0.4){
        //printf("occupazione ram non rilevante esco\n");
        return NULL;}
        
        
        
        

    

    

        get_info_tot((void*)struttura_cpu,1);

        get_info_proc(pid_da_arr,(void*)struttura_proc,1);  // prima della sleep

        sleep(3);
       
        get_info_tot((void*)struttura_cpu,0);

        get_info_proc(pid_da_arr,(void*)struttura_proc,0);

        //printf("printo dati raccolti per processo\n");

        //printf("processo %d user time == %ld,user time after == %ld\n",struttura_proc->PID,struttura_proc->time_u,struttura_proc->time_u_after);

        int ris=(int)(struttura_proc->time_u_after-struttura_proc->time_u);
       //int ris_cu=(int)(struttura_proc->time_cu_after-struttura_proc->time_cu); //child
       
        //printf("DIFFERENZA Utime_processo %d== %d\n",struttura_proc->PID,ris);

        int ris_2=(int)struttura_proc->time_k_after-(int)struttura_proc->time_k;                           //  -- se voglio conteggiare il k time
       // int ris_ck=(int)(struttura_proc->time_ck_after-struttura_proc->time_ck); //child              
        //printf("DIFFERENZA Ktime_processo == %d\n",ris_2);

        ris=ris+ris_2;

        int ris_1=struttura_cpu->cpu_usage_after-struttura_cpu->cpu_usage;
        //printf("DIFFERENZA time CPU == %d\n",ris_1);

        float per=(float)ris/(float)ris_1;
        
        //printf("nome %s  pid %d  stato %c cpu usage %0.3f percentuale ram  == %0.2f, memoria virtuale  == %0.2fKb\n",struttura_proc->nome,struttura_proc->PID,struttura_proc->stato,per*100,fis,vir);
        printf(" %s              %d            %c                 %0.3f            %0.2f                   %0.2fKb\n",struttura_proc->nome,struttura_proc->PID,struttura_proc->stato,per*100,fis,vir);

        //printf(" processo %d cpu usage == %0.3f \n",struttura_proc->PID,per*100);
   

      
        

        return NULL;
     

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
    //printf("percorso cercato== %s\n",percorso);
    }
    FILE* fd=fopen(percorso,"r");
    if(fd==NULL){
        printf("errore apertura il processo non esiste\n");
        //exit(EXIT_FAILURE);
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
    //printf("pid %d %s stato %c    questo %lu %lu %d %d \n",valori_i[0],nome_p,stato,valori_lu[4],valori_lu[5],valori_i[6],valori_i[7]);
    
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

void get_info_tot(void* info_sis, int t){    ///////// ? NON SEMPRE 

    cpu_info_t* dati=(cpu_info_t*)info_sis;

    int v1=0,v2=0,v3=0,v4=0,v5=0,v6=0,v7=0;
    char* n=malloc(5);
    FILE* fd=fopen("/proc/stat","r");
    if(fd==NULL){
        printf("errore apertura\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fd,"%s %d %d %d %d %d %d %d \n",n,&v1,&v2,&v3,&v4,&v5,&v6,&v7);
    int sum=v1+v2+v3+v4+v5+v6+v7;   //somma cpu giusta                             ricontrollato giusto
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
        *(m + r) = malloc(20 * sizeof(char)); 
    }
    return m;

}

int popola_ris(char** m,int n, struct dirent** files_1){             // SI
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

                if(!r){
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
void get_mem_proc(float*fisica,float* virtuale,char*pid_p,float Ram_v){                  // SI  


    char* percorso=malloc(20);
    memset((void*)percorso,0,20);
    const char* pid_1=pid_p;
    strcat(percorso, "/proc/");
    strcat(percorso, pid_1);
    strcat(percorso, "/status");
    //printf("percorso cercato per memoria processo == %s\n",percorso);

    FILE* file = fopen(percorso, "r");
    if(file==NULL){
        printf("errore apertura file proc_mem\n");
        exit(EXIT_FAILURE);
    }                                               
    char line[128];
    int res=0;
    

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){        // fisica      ///////    
            res = parseLine(line);
            break;
        }
    }
    fclose(file);

    FILE* file_1 = fopen(percorso, "r");
    if(file_1==NULL){
        printf("errore apertura  1 file proc_mem\n");          // non riesco a prendere anche la virtuale , -- RIPROVALO (già modificato)
        exit(EXIT_FAILURE);
    }
    
    char line_1[128];
    int res_1=0;
       while (fgets(line_1, 128, file_1) != NULL){
        if (strncmp(line_1, "VmSize:", 7) == 0){     // virtuale                 
            res_1 = parseLine(line_1);
            break;
        }
    }
    
    fclose(file_1);

    //printf("Memoria fisica proc == %d\n",res);

    float vera_f=(float)(res/1024);


    *fisica=(float)(vera_f/Ram_v)*100;

    //printf("Memoria virtuale proc  == %d\n",res_1);
    *virtuale=res_1;
    

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

int get_mem_sistema(){                                    // SI
    struct sysinfo memInfo;

    sysinfo (&memInfo);

    int32_t p=0;
    int32_t after=0;
    p=(uint64_t)memInfo.totalram;

    after = ((uint64_t)memInfo.totalram * memInfo.mem_unit)/1024;          // giusto valore totale (mem total (RAM))

    //printf("VEDIAMO == %d\n",after);
    return after;

}

void get_dettagli_cpu(){    // proc/cpuinfo

    char*percorso="/proc/cpuinfo";
    //printf("crco percorso == %s\n",percorso);

    FILE* file = fopen(percorso, "r");
        if(file==NULL){
            printf("errore apertura file proc_mem\n");
            exit(EXIT_FAILURE);
        }                                               
        char line[128];
        int res=0;
         while (fgets(line, 128, file) != NULL){
            //printf("riga == %s\n",line);
        if (strncmp(line, "model name:", 10) == 0){       
            printf("%s \n",line);
            continue; 
        }
        if (strncmp(line, "cpu MHz", 7) == 0){       
            printf("%s \n",line);
            continue; 
        }
        if (strncmp(line, "cache size:", 10) == 0){       
            printf("%s \n",line);
            continue; 
        }
         if (strncmp(line, "TLB size", 8) == 0){       
            printf("%s \n",line);
            break;
        }
        
    }

}
void get_fd_programma(struct dirent** file_d,char* process){

    char* percorso=malloc(20);
    memset((void*)percorso,0,20);
    const char* pid_1=process;
    strcat(percorso, "/proc/");
    strcat(percorso, pid_1);
    strcat(percorso, "/fd");

     int n = scandir(percorso, &file_d, NULL, alphasort); // n = quanti

        if (n < 0) {
            printf("errore richiesta file in proc ( non accessibile o terminato\n");
            //exit(EXIT_FAILURE);
        }

     for (int i = 0; i < n; i++) {
        struct dirent *file_dir = file_d[i];
        printf("file descriptor aperti : %s\n",file_dir->d_name);
     }


}
