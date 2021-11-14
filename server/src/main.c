#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"




extern PlayersInfo* players_info;
extern int start;
extern int server_socket;


int main(int argc, char *argv[]){
  // Se define una IP y un puerto

  // Se crea el servidor y se obtienen los sockets de ambos clientes.
  //PlayersInfo * players_info = prepare_sockets_and_get_clients(IP, PORT);
  pthread_t thread_id;




  pthread_create(&thread_id, NULL, prepare_sockets_and_get_clients, NULL);
  wait_ready();
  printf("EMPEZÓ EL JUEGO\n");
  

  pthread_cancel(thread_id);

  printf("TERMINO EL JUEGO\n");

  for(int i = 0; i<4;i++){
    if(players_info->sock[i]!= 0){
    server_send_message(players_info->sock[i], 10, "");
    free(players_info->players[i]);
    }    
  }
  free(players_info);
  //close(server_socket);
  sleep(3);

  return 0;
}
