#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "comunication.h"
#include <pthread.h>


typedef struct player{
  char* name;
  int sock;
  int farmers;
  int miners;
  int ings;
  int army;
} Player;


typedef struct players_info{
  int sock[4];
  int conectados;
  int listos;
  int start;
  Player* players[4];
} PlayersInfo;


void wait_ready();
void *prepare_sockets_and_get_clients();



