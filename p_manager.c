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

int main(int argc,char* argv[]){

    // ORGANIZZO VARIABILI
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
    



       
        //printf("==> Inizio richiesta informazioni\n");
        printf("eseguo comando == %s\n",richiesta);
        
        //----------------------------------------------------------------------------------------
        // getcwd per la directory corrente

        struct dirent **files;
        int n = scandir("/proc", &files, NULL, alphasort);
        //int n = scandir("/home/paolascema/Scrivania/", &files, NULL, alphasort);

        // alloco 
        file_in_proc = malloc(n*sizeof(char*));
        for (int r=0;r < n;r++){
            *(file_in_proc+r) = malloc(20*sizeof(char));    // 20 abbastanza arbitrario (dovresti aggiustare)
        }


        //controllo risultati

        if (n < 0) {
            printf("errore richiesta file inp proc");
            exit(EXIT_FAILURE);
        }

        /* vedo cosa trovo */
        for (int i = 0; i < n; i++) {

            /* associo risultato a struttura  ( guarda altre voci )*/

            struct dirent *file_dir = files[i];
            file_in_proc[i]=file_dir->d_name;


            // fai funzione apposita magari dove settare regex
            regex_t regex;
            r = regcomp(&regex, "[0-9]", 0);
            if (r) {
                fprintf(stderr, "Could not compile regex\n");
                exit(1);
            }


            r = regexec(&regex, file_dir->d_name, 0, NULL, 0);
                if( !r ){
                        //puts("Match");
                        printf("%s\n", file_in_proc[i]);
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

        /* libero mem (fare funzione a parte) */
        for (int i = 0; i < n; i++) {
            free(files[i]);
        }
        free(files);

    


        //printf("Files in Directory are: \n");
        //execl("/bin/ls","/home/paolascema/Scrivania/ls","-l",0);    // funge
        //deve essere eseguitada un thread/processo altrimenti mi fa terminare il programma
        // non posso cambiare la directory

        

    }




    printf("entrato comando quit \n");
    fflush(stdout);
    


}
