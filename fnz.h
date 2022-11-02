


#define QUIT_COMMAND  "QUIT"
#define STOPPA "stop"
#define RIPRENDI "resume"
#define TERMINA "termina"
#define KILLA "kill"
#define MAX_PID  10


void get_mem_proc(float* fisica,float*virtuale,char* pid_p,float Ram_v);  // prende memoria fisica e virtuale singolo processo (pid_p)

int get_mem_sistema(); // prende solo ram sistema

void get_dettagli_cpu();    // proc/cpuinfo

int parseLine(char* line);

char** alloca_m(int n);  // aloca matrice s

void printa(void* struttura);

int popola_ris(char** m,int n,struct dirent** files_1); // seleziona solo pid da /proc

void get_info_proc(char* pid,void* struttura,int t);  // prende info singolo processo

//void* get_percent_one(char* pid_da_arr,void* struttura_cpu,void* struttura_proc , void* stat_struct);  // ricava percentuali (cpu) di uno
void* get_percent_one(void* arg_struct);  // ricava percentuali (cpu) di uno

void get_info_tot(void* info_sis, int t);  // usata per calcolare percentuale cpu di un processo, prendo uso cpu prima e dopo la sleep

void get_fd_programma(struct dirent** file_d,char* process);