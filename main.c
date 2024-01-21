#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define RAYMATH_IMPLEMENTATION

//SCREEN 
#define S_DIMY 1000
#define S_DIMX (S_DIMY * 16) / 9      // Dimensione schermo (16 : 9)

//CELLS
#define C_DIM 8                       // Dimensione in pixel casella
#define C_NUMX (int) S_DIMX / C_DIM   // Numero caselle asse X
#define C_NUMY (int) S_DIMY / C_DIM   // Numero caselle asse Y
#define DST 0.75                      // Peso della distanza tra due celle consecutive. Peso più piccolo calcolo più veloce ma risultato meno preciso
                                      // 0.95 sembra essere un ottimo valore

//RANDOMNESS
#define R_RANDOMNESS 3               // 1/R_RANDOMNESS è la possibilità che una casella sia un blocco 

#define REG_POINT                     // Se definito premendo "R" è possibile rigenerare randomicamente la posizione di Start e End 
//#define PRINT_FINAL_ROUTE           // Se definito stampa a terminale una lista di tutti i nodi attraversati dalla route finale
//#define PRINT_TRASH                 // Se definito vengono stampati tutti gli indirizzi dei nodi eliminati (debug)
#define RANDOM_SPAWN false            // Se vero Start e End vengono generati randomicamente
#define STEP_DST true                 // Se vero la funzione distanza sarà applicata ad una griglia, se falso teorema di pitagora

// La tabella sotto riportata mostra come usare una funzione che calcola la distanza tra due punti su una griglia 
// dia risultati nettamente migliori rispetto ad una funzione che usa il teorema di pitagora.
//
//  -> Nel primo test la dimensione delle caselle è 16 e la randomness 10 
//  -> Nel secondo test la dimensione delle caselle è 8 e la randomness 4
//
// STEP_DST   true : false
// -----------------------------------------------
// seed 100 | 3.4 : 7.0 | 33.1 : 40.1 | seconds 
// seed 69  | 3.3 : 4.5 | 2.3 : 3.4   | seconds 
// seed 420 | 0.9 : 1.3 | 3.9 : 7.7   | seconds
// seed 7   | 3.1 : 5.3 | 2.0 : 3.2   | seconds 

typedef struct Node{

  Vector2 cord;
  float fscore;
  struct Node * next;

} Node_t;

typedef Node_t* ptr_node;


enum Cells{
  C_EMPTY,
  C_BLOCK,
  C_START,
  C_END,
  C_ROUTE,    //Route già percorsa
  C_OPEN_R,   //Route ancora percorribile
  C_FINAL_R,  //Route Finale
};

// The function insert a 1 in random position of the array:
//  -> map is the array that you want to modify
//  -> x_max is the part of the arrray (from 0 to x_max) that is going to be modified on x axes
//  -> y_max is the part of the arrray (from 0 to y_max) that is going to be modified
// Return the number of added obstacles. 
int RandomizeObstacles(int map[][C_NUMY], int x_max, int y_max, int randomness);

// Return a random point not occupied in the map
Vector2 RandomizePoint(int map[][C_NUMY], int x_max, int y_max);

// Funzioni per inizializzare l'array con il valore passato
void initializeArrayF(float arr[][C_NUMY], int x_max, int y_max, int value);
void initializeArrayV(Vector2 arr[][C_NUMY], int x_max, int y_max, Vector2 vector);

// Return distance of two points
float dst(int x1, int y1, int x2, int y2);

// Funzioni liste
ptr_node generateNode(Vector2 cord, float fscore, ptr_node next); // Genera un nodo
ptr_node rmNodeByCord(ptr_node openSet, Vector2 ob_cord);         // Rimuove un nodo con delle determinate cordinate
ptr_node addInOrder(ptr_node openSet, ptr_node new_node);         // Aggiunge un nodo in modo che la lista sia in ordine decrescente di fscore
int freeList(ptr_node openSet);                                   // Libera la lista

// Disegna il percorso finale nella mappa
void drawFinalRouteRec(int map[][C_NUMY], Vector2 start, Vector2 end, int *cont);

//Globals
Vector2 cmfrom[C_NUMX][C_NUMY];
float gscore[C_NUMX][C_NUMY];
float fscore[C_NUMX][C_NUMY];
 
int main(int argc, char * argv[]){

  //IF PROCESS IS RUN AS DEAMON DONT PRINT LOG
  if(argc > 1){
    if(strcmp(argv[1],"true") == 0) SetTraceLogLevel(LOG_ERROR); 
  }

  int seed = time(NULL);
  //int seed = 420;

  printf("[INFO] Random seed: %d\n", seed);
  srand(seed); //-> Seed randomico

  int map[C_NUMX][C_NUMY] = {0};
  int n_obs = RandomizeObstacles(map, C_NUMX, C_NUMY, R_RANDOMNESS);

  printf("[INFO] %d obstacles added\n", n_obs);

  #if RANDOM_SPAWN
    Vector2 start = RandomizePoint(map, C_NUMX, C_NUMY);
    Vector2 end = RandomizePoint(map, C_NUMX, C_NUMY);
  #endif

  #if !RANDOM_SPAWN
    Vector2 start = {0.0, 0.0};
    Vector2 end = {C_NUMX - 1.0, C_NUMY - 1.0};
  #endif

  printf("[INFO] Start -> (%f %f)\n", start.x, start.y);
  map[(int) start.x][(int) start.y] = C_START;
  printf("[INFO] End -> (%f %f)\n", end.x, end.y);
  map[(int) end.x][(int) end.y] = C_END;

  bool pause = true;              // A* in pausa
  bool finish = false;            // A* ha finito
  bool draw_final_route = true;   // E' necessario disegnare la route alla fine (una volta disegnata questo valore viene posto a false)
  bool exist_route = true;        // Esisste la route (se non esiste nessuna route questo valore viene posto a false)

  // A* configuration\s
  ptr_node openSet = generateNode(start, dst(start.x, start.y, end.x, end.y), NULL);

  initializeArrayF(gscore, C_NUMX, C_NUMY, -1.0);
  initializeArrayF(fscore, C_NUMX, C_NUMY, -1.0);
  initializeArrayV(cmfrom, C_NUMX, C_NUMY, (Vector2) {-1, -1});
  
  gscore[(int) start.x][(int) start.y] = 0;
  fscore[(int) end.y][(int) end.y] = dst(start.x, start.y, end.x, end.y);

  // SetConfigFlags(FLAG_WINDOW_RESIZABLE); -> window resizable
  InitWindow(S_DIMX, S_DIMY, "A*");
  
  while(!WindowShouldClose()){
      
    #ifdef REG_POINT
      if(IsKeyPressed(KEY_R)){
        map[(int) start.x][(int) start.y] = C_EMPTY;
        map[(int) end.x][(int) end.y] = C_EMPTY;

        start = RandomizePoint(map, C_NUMX, C_NUMY);
        map[(int) start.x][(int) start.y] = C_START;
        end = RandomizePoint(map, C_NUMX, C_NUMY);
        map[(int) end.x][(int) end.y] = C_END; 

        printf("[INFO] Start -> (%f %f)\n", start.x, start.y);
        printf("[INFO] End -> (%f %f)\n", end.x, end.y);

        openSet->cord = start;
        openSet->fscore = dst(start.x, start.y, end.x, end.y);
      }
    #endif

    if(IsKeyPressed(KEY_ENTER)){ 
      pause = !pause;
    }

    BeginDrawing();
    ClearBackground(GRAY);

    for(int i = 0; i <= C_NUMX; i++){
        DrawLine(i * C_DIM, 0, i * C_DIM, S_DIMY, BLACK);
      }

    for(int i = 0; i <= C_NUMY; i++){
        DrawLine(0, i * C_DIM, S_DIMX, i * C_DIM, BLACK);
      }
    
    Color cl;
    for(int i = 0; i < C_NUMX; i++){
      for(int j = 0; j < C_NUMY; j++){
        //Selezione colore cella
        switch (map[i][j]){
          case C_BLOCK:
            cl = BLACK;
            break;
          case C_START:
            cl = RED;
            break;
          case C_END:
            cl = GREEN;
            break;
          case C_ROUTE:
            cl= SKYBLUE;
            break;
          case C_OPEN_R:
            cl = DARKBLUE;
            break;
          case C_FINAL_R:
            cl = GOLD;
            break;
          default:
            cl = BLANK;
        }

        DrawRectangle(i * C_DIM, j * C_DIM, C_DIM, C_DIM, cl);
        /*
        if(j == 0 && map[i][j] == C_OPEN_R){
          printf("(%d %d)\n", i, j);  
        }

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%d", j);
        DrawText(buffer, i * C_DIM, j * C_DIM, C_DIM / 2, PURPLE);
        */
      }
    }

    EndDrawing();

    if(draw_final_route && finish && exist_route){
      int len = 0;
      drawFinalRouteRec(map, start, end, &len);
      printf("[INFO] Nodi totali: %d\n", len);
      printf("[INFO] Nodi idealmente minimi: %d\n", abs(start.x - end.x) + abs(start.y - end.y) - 1);
      draw_final_route = false;
      int numEnds = freeList(openSet);
      printf("[INFO] Elimino %d elementi dalla lista\n", numEnds);
      //return 0;
    }

    /* A* Alghoritm */

    //printf("%p %d %d\n", openSet, pause, finish);

    if(openSet != NULL && (!pause && !finish)){
      ptr_node curr = openSet;

      // Condizione finale terminazione algoritmo
      if(Vector2Equals(curr->cord, end)){
        cmfrom[(int) end.x][(int) end.y] = cmfrom[(int) curr->cord.x][(int) curr->cord.y] ;
        printf("[INFO] Route founded!\n");
        finish = true;
        //return 0;
      }
      else{

        openSet = curr->next;

        // Aggiungiamo ogni vicino alla lista
        for(int i = 0; i < 4; ++i){

          Vector2 neighbor_cord = {curr->cord.x, curr->cord.y};

          switch (i % 4) {
            case 0:
              neighbor_cord.x += 1;
              break;
            case 1:
              neighbor_cord.x -= 1;
              break;
            case 2: 
              neighbor_cord.y += 1;
              break;
            case 3:
              neighbor_cord.y -= 1;
              break;
            default:
              break;
          }

          if((neighbor_cord.y < 0 || neighbor_cord.y >= C_NUMY) || (neighbor_cord.x < 0 || neighbor_cord.x >= C_NUMX)) continue;

          int elem = map[(int) neighbor_cord.x][(int) neighbor_cord.y];

          if(elem != C_BLOCK){
            float distance = DST;
            float tentative_gScore = gscore[(int) curr->cord.x][(int) curr->cord.y]  + distance;

            //printf("%f %f\n",tentative_gScore, gscore[(int) neighbor_cord.x][(int) neighbor_cord.y]);

            if(tentative_gScore < gscore[(int) neighbor_cord.x][(int) neighbor_cord.y] || gscore[(int) neighbor_cord.x][(int) neighbor_cord.y] < 0){
              cmfrom[(int) neighbor_cord.x][(int) neighbor_cord.y] = curr->cord;
              gscore[(int) neighbor_cord.x][(int) neighbor_cord.y] = tentative_gScore;
              fscore[(int) neighbor_cord.x][(int) neighbor_cord.y] = tentative_gScore + dst(neighbor_cord.x, neighbor_cord.y, end.x, end.y);

              if(elem == 5){ //-> elemento già in openSet
               //elimino il vecchio elemento
                ptr_node openSet = rmNodeByCord(openSet, neighbor_cord);
              }

              ptr_node new_node = generateNode(neighbor_cord,  fscore[(int) neighbor_cord.x][(int) neighbor_cord.y] , NULL);
              if(new_node == NULL){
                printf("Malloc fallita\n");
                return 1;
              }
              openSet = addInOrder(openSet, new_node);

              if(elem != C_START && elem != C_END){ 
                map[(int) neighbor_cord.x][(int) neighbor_cord.y] = C_OPEN_R;

                /*
                for(int i = 0; i < C_NUMX; ++i){
                    printf("%d|", map[i][0]);
                  }
                printf(" -> (%d %f) %d\n", (int) neighbor_cord.y, neighbor_cord.y, C_NUMY);
                */

              }
            
            } // end if gtent
          }// end if(elem != 1)
        }// end for vicini
        
        if(map[(int) curr->cord.x][(int) curr->cord.y] != C_START && map[(int) curr->cord.x][(int) curr->cord.y] != C_END){
          map[(int) curr->cord.x][(int) curr->cord.y] = C_ROUTE;
        }
        free(curr);

      }//End else 
    }//end if
    
    // Se la lista di percorsi avviabili e nulla ma l'algoritmo non è ancora arrivato alla fine significa che è impossibile
    // raggiungere la fine dal punto desiderato.
    if(openSet == NULL && finish == false){
      printf("[INFO] Impossible to calculate a route. I'm stuck step function!\n");
      finish = true;
      exist_route = false;
      int numEnds = freeList(openSet);
      printf("[INFO] Elimino %d elementi dalla lista.\n", numEnds);

    }

  }//end while 

  CloseWindow();

  return 0;
}

int RandomizeObstacles(int map[][C_NUMY], int x_max, int y_max, int randomness){

  int count = 0;

  for(int i = 0; i < C_NUMX && i < x_max; ++i){
    for(int j = 0; j < C_NUMY && j < y_max; ++j){
     if(rand() % randomness == 0){
        map[i][j] = C_BLOCK;
        count++;
      }
    }
  }
  
  return count;
}

Vector2 RandomizePoint(int map[][C_NUMY], int x_max, int y_max){
  int x, y;

  do{

    x = (x_max < C_NUMX) ? rand() % C_NUMX : rand() % C_NUMX; 
    y = (y_max < C_NUMY) ? rand() % C_NUMY : rand() % C_NUMY; 

  } while(map[x][y] != C_EMPTY);

  Vector2 ret = {(float) x, (float) y};
  return ret;
}

#if !STEP_DST
  float dst(int x1, int y1, int x2, int y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
  }
#endif

#if STEP_DST
  float dst(int x1, int y1, int x2, int y2){
    return abs(x1 - x2) + abs(y1 - y2) - 1;
  }
#endif

ptr_node generateNode(Vector2 f_cord, float f_fscore, ptr_node f_next){

  ptr_node p = malloc(sizeof(Node_t));
  if (p != NULL){
    p->cord = f_cord;
    p->fscore = f_fscore;
    p->next = f_next;
  }
  return p;
  
}

ptr_node rmNodeByCord(ptr_node openSet, Vector2 ob_cord){

  if(openSet == NULL)
    return NULL;

  ptr_node prec = NULL;
  for( ptr_node temp = openSet; temp->next != NULL; temp = temp->next){
    if(Vector2Equals(temp->cord, ob_cord)){
      if(prec == NULL) return temp;
      else {
        prec->next = temp->next;
        free(temp);
      }
    }  
    prec = temp;
  }

  return openSet;
}

ptr_node addInOrder(ptr_node openSet, ptr_node new_node){
    
  if(openSet == NULL){
    return new_node;
  }

  ptr_node prec = NULL;
  ptr_node temp = openSet;

  for(;temp != NULL && (temp->fscore <= new_node->fscore); temp = temp->next){
    prec = temp;
  }

  if(temp == NULL){
    //Aggiungo il nodo in coda
    prec->next = new_node;
  }
  else if(temp == openSet){
    new_node->next = openSet;
    return new_node;
  }
  else{
    new_node->next = temp;
    prec->next = new_node;
  }  

  return openSet;
}

void initializeArrayF(float arr[][C_NUMY], int x_max, int y_max, int value){

  for(int i = 0; i < C_NUMX && i < x_max; ++i){
    for(int j = 0; j < C_NUMY && j < y_max; ++j){
      arr[i][j] = value;
    } 
  }
}

void initializeArrayV(Vector2 arr[][C_NUMY], int x_max, int y_max, Vector2 vector){

  for(int i = 0; i < C_NUMX && i < x_max; ++i){
    for(int j = 0; j < C_NUMY && j < y_max; ++j){
      arr[i][j] = vector;
    } 
  }
}

void drawFinalRouteRec(int map[][C_NUMY], Vector2 start, Vector2 v, int *cont){
   
  if(!Vector2Equals(start, v) &&  map[(int) v.x][(int) v.y] != C_END) map[(int) v.x][(int) v.y] = C_FINAL_R;
  
  #ifdef PRINT_FINAL_ROUTE
    printf("(%d,%d)\n", (int) v.x,(int) v.y);
  #endif
  *cont +=1;

  Vector2 nextV = cmfrom[(int) v.x][(int) v.y];
  Vector2 nextNextV = cmfrom[(int) nextV.x][(int) nextV.y];

  if(Vector2Equals(v, nextNextV)){
    printf("[ERR] Path in loop: ");
    printf("(%d %d)\n",(int) v.x, (int) v.y);
  }else{
    if(!Vector2Equals(v, start)) drawFinalRouteRec(map, start, cmfrom[(int) v.x][(int) v.y], cont);
  }

}

int freeList(ptr_node openSet){
  
  if(openSet == NULL) return 0;
  
  int cont = 0;
  ptr_node temp = NULL;
  for(ptr_node trash = openSet; trash->next != NULL; ++cont){
    temp = trash->next;
    free(trash);
    #ifdef PRINT_TRASH
     printf("trash -> %p\n", trash);
    #endif /* ifdef PRINT_TRASH */
    trash = temp;
  }

  return cont;

}
