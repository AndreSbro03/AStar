#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PATH "./A_path_finder"
#define MTRUE "true"
#define MFALSE "false"

int main(int argc, char *argv[]){

  if(argc < 2){
    printf("[ERR] To few arguments\n");
    return -1;
  }

  if((strcmp(argv[1],MTRUE) != 0) && (strcmp(argv[1],MFALSE) != 0)){
    printf("[ERR] Invalid arguments, only %s or %s accepted. \n", MTRUE, MFALSE);
    return -1;
  }


  pid_t pid;
  pid = fork();

  if(pid == 0){

    if(strcmp(argv[1],MTRUE) == 0){
      printf("[INFO] Processo deamon creato.\n");

      execl(PATH, PATH, argv[1], NULL);
    }
    else{
      printf("[INFO] Processo non deamon creato.\n");
      execl(PATH, PATH, NULL); 
    }

    printf("[ERR] Non è stato possibile avviare: %s\n", PATH);
    printf("[INFO] Exit\n");
    return -1;
  }
  
  if(strcmp(argv[1],MFALSE) == 0){
    printf("[INFO] Attesa terminazione processo deamon.\n");
    waitpid(pid, NULL, 0);
  }
  printf("[INFO] Run è terminato con successo.\n");
  printf("[INFO] Exit\n");
  
  return 0;
}
