#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <regex.h> 

#include <sys/types.h>
#include <sys/dir.h>
//
#define QUIT_COMMAND  "QUIT"

char** alloca_m(int n);
int popola_ris(char** m,int n,struct dirent** files_1);

int main(int argc,char* argv[]){

    // ORGANIZZO VARIABILI
    struct dirent **files;
    int r;
    char** file_in_proc;
    char *quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    printf("lunghezza quit command == %ld\n",quit_command_len);

    char*arg1=malloc(20);
    char*arg2=malloc(20);
    if (argc > 1) arg1 = argv[1];
	if (argc > 2) arg2 = argv[2];


    while(1){

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

        printf("stampo file salvati in proc (quelli numerici)\n");
        
        for (int j=0;j<quanti;j++){

            printf("file == > %s\n", file_in_proc[j]);
            

        }

        printf("FINE STAMPA\n");

        // free 
        for (int i = 0; i < n; i++) {
            free(files[i]);
        }
        free(files);
        printf("DEALLOCATO\n");



        //printf("Files in Directory are: \n");
        //execl("/bin/ls","/home/paolascema/Scrivania/ls","-l",0);    // funge
        //deve essere eseguitada un thread/processo altrimenti mi fa terminare il programma
        // non posso cambiare la directory

    }




    printf("entrato comando quit \n");
    fflush(stdout);
    


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
