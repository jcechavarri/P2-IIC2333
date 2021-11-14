#include <pthread.h>
#include "conection.h"

//LINKS REFERENCIAS:
//https://www.man7.org/linux/man-pages/man2/socket.2.html
//https://man7.org/linux/man-pages/man7/socket.7.html
//https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
//https://man7.org/linux/man-pages/man2/setsockopt.2.html
//https://linux.die.net/man/3/htons
//https://linux.die.net/man/3/inet_aton
//https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
//https://www.man7.org/linux/man-pages/man2/bind.2.html
//https://www.man7.org/linux/man-pages/man2/accept.2.html

PlayersInfo* players_info;
pthread_mutex_t lock;
int server_socket;

int start = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

void *lobby(void *i){
  int n = *((int *) i);
  free(i);
  printf("Lobby del %i\n\n",n);

  char *message = "Bienvenido Cliente!!\n";
  server_send_message(players_info->sock[n], 1, message);

  int msg_code = server_receive_id(players_info->sock[n]);
  char *client_name = server_receive_payload(players_info->sock[n]);
  printf("%s\n", client_name);
  players_info->players[n] = malloc(sizeof(Player));
  players_info->players[n]->name = malloc(sizeof(client_name));
  strcpy(players_info->players[n]->name, client_name);
  free(client_name);
  
  message = "Reparta a sus aldeanos\n";
  server_send_message(players_info->sock[n], 2, message);

  int a = server_receive_id(players_info->sock[n]);
  char* payload_1 = server_receive_payload(players_info->sock[n]);
  free(payload_1);

  players_info->players[n]->farmers = a;
  printf("%i\n",players_info->players[n]->farmers);

  int m = server_receive_id(players_info->sock[n]);
  char* payload_2 = server_receive_payload(players_info->sock[n]);
  free(payload_2);

  players_info->players[n]->miners = m;
  printf("%i\n",players_info->players[n]->miners);

  int ing = server_receive_id(players_info->sock[n]);
  char* payload_3 = server_receive_payload(players_info->sock[n]);
  free(payload_3);

  players_info->players[n]->ings = ing;
  printf("%i\n",players_info->players[n]->ings);

  int g = server_receive_id(players_info->sock[n]);
  char* payload_4 = server_receive_payload(players_info->sock[n]);
  free(payload_4);

  players_info->players[n]->army = g;
  printf("%i\n",players_info->players[n]->army);


  pthread_mutex_lock(&lock);
  players_info->listos ++;
  pthread_mutex_unlock(&lock);


  if(n==0){
    while(1){
      message = "Aprete ENTER para empezar el juego";
      server_send_message(players_info->sock[n], 3, message);
      msg_code = server_receive_id(players_info->sock[n]);
      char* enter = server_receive_payload(players_info->sock[n]);
      free(enter);

      
      if(players_info->conectados==players_info->listos && players_info->conectados>1){
        players_info->start = 1;
        message = "Ha comenzado el juego";
        for(int i = 0; i<4;i++){
          if(players_info->sock[i]!= 0){
          server_send_message(players_info->sock[i], 0, message);
          }
        }
        set_ready(1);
        printf("setreadi\n");
        return 0;
      }else{
        if (players_info->conectados ==1){
          message = "Tienen que haber 2 o más jugadores";
          server_send_message(players_info->sock[n], 0, message);
        }else{
          message = "Tiene que esperar a que todos los clientes esten listos";
          server_send_message(players_info->sock[n], 0, message);
        }
      }
    }
  }
}

void *prepare_sockets_and_get_clients(){
  char * IP = "0.0.0.0";
  int port = 8080;
  // Se define la estructura para almacenar info del socket del servidor al momento de su creación
  struct sockaddr_in server_addr;

  // Se solicita un socket al SO, que se usará para escuchar conexiones entrantes
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
  int opt = 1;
  int ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  // Se guardan el puerto e IP en la estructura antes definida
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(IP, &server_addr.sin_addr);
  server_addr.sin_port = htons(port);

  // Se le asigna al socket del servidor un puerto y una IP donde escuchar
  int ret2 = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Se coloca el socket en modo listening
  int ret3 = listen(server_socket, 1);

  // Se definen las estructuras para almacenar info sobre los sockets de los clientes
  struct sockaddr_in client1_addr;
  struct sockaddr_in client2_addr;
  struct sockaddr_in client3_addr;
  struct sockaddr_in client4_addr;
  socklen_t addr_size = sizeof(client1_addr);


  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  players_info = malloc(sizeof(PlayersInfo));
  //players_info->sock = calloc(4,sizeof(int));

  players_info->listos = 0;  
  players_info->conectados = 0;
  players_info->start = 0;
  players_info->sock[0] = 0;
  players_info->sock[1] = 0;
  players_info->sock[2] = 0;
  players_info->sock[3] = 0;

  

  // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el n° de otro socket asignado para la comunicación
  printf("Esperando al 1\n");
  pthread_t thread_id;
  players_info->sock[0] = accept(server_socket, (struct sockaddr *)&client1_addr, &addr_size);
  players_info->conectados++;
  printf("Thread al 1\n");
  int *arg1 = malloc(sizeof(*arg1));
  *arg1 = 0;
  pthread_create(&thread_id, NULL, lobby, (void *) arg1);
  pthread_detach(thread_id);
  //free(*arg1);
  //crea otro
  printf("Esperando al 2\n");
  players_info->sock[1] = accept(server_socket, (struct sockaddr *)&client2_addr, &addr_size);
  players_info->conectados++;
  int *arg2 = malloc(sizeof(*arg2));
  *arg2 = 1;
  pthread_create(&thread_id, NULL, lobby, (void *) arg2);
  pthread_detach(thread_id);
  //free(*arg2);
  //crea otro
  printf("Esperando al 3\n");
  players_info->sock[2] = accept(server_socket, (struct sockaddr *)&client3_addr, &addr_size);
  players_info->conectados++;
  int *arg3 = malloc(sizeof(*arg3));
  *arg3 = 2;
  pthread_create(&thread_id, NULL, lobby, (void *) arg3);
  pthread_detach(thread_id);
  //free(*arg3);
  //crea otro
  printf("Esperando al 4\n");
  players_info->sock[3] = accept(server_socket, (struct sockaddr *)&client4_addr, &addr_size);
  players_info->conectados++;
  int *arg4 = malloc(sizeof(*arg4));
  *arg4 = 3;
  pthread_create(&thread_id, NULL, lobby, (void *) arg4);
  pthread_detach(thread_id);
  //free(*arg4);

  //return sockets_clients;
}






void wait_ready()
{
  pthread_mutex_lock(&ready_mutex);
  while(!start) {
    pthread_cond_wait(&ready_cond, &ready_mutex);
  }
  pthread_mutex_unlock(&ready_mutex);
}


void set_ready(int ready)
{
  pthread_mutex_lock(&ready_mutex);
  start = ready;

  pthread_cond_signal(&ready_cond);
// or using pthread_cond_broadcast(&ready_cond);

  pthread_mutex_unlock(&ready_mutex);

}