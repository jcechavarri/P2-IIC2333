#include <unistd.h>
#include <stdio.h>
#include "conection.h"
#include "comunication.h"

char * get_input(){
  char * response = malloc(20);
  int pos=0;
  while (1){
    char c = getchar();
    if (c == '\n') break;
    response[pos] = c;
    pos++;
  }
  response[pos] = '\0';
  return response;
}


int main (int argc, char *argv[]){
  //Se obtiene la ip y el puerto donde está escuchando el servidor (la ip y puerto de este cliente da igual)
  char * IP = "0.0.0.0";
  int PORT = 8080;

  // Se prepara el socket
  int server_socket = prepare_socket(IP, PORT);

  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1){
    int msg_code = client_receive_id(server_socket);
    if (msg_code==0){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);

    }
    if (msg_code == 1) { //Recibimos un mensaje del servidor
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
      printf("Dame tu nombre:\n");
      char * nombre = get_input();
      client_send_message(server_socket, 0, nombre);
      free(nombre);
    }

    if (msg_code == 2) { //Recibimos un mensaje que proviene del otro cliente
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);

      printf("Agricultores:");
      int a = getchar() - '0';
      getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin
      client_send_message(server_socket, a, "");

      printf("Mineros:");
      int m = getchar() - '0';
      getchar();
      client_send_message(server_socket, m, "");

      printf("Ingenieros:");
      int i = getchar() - '0';
      getchar();
      client_send_message(server_socket, i, "");

      printf("Guerreros:");
      int g = getchar() - '0';
      getchar();
      client_send_message(server_socket, g, "");
    }

    if (msg_code == 3) { 
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
      getchar();
      char* response = "response";
      client_send_message(server_socket, 0, response );
    }
    printf("------------------\n");

    if (msg_code == 10) { 
      break;
    }
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);

  return 0;
}
