#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  bind(server_fd, (struct sockaddr*)&address, sizeof(address));

  listen(server_fd, 3);
  printf("Server listening on PORT: %d\n", PORT);

  while(1) {
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    read(client_fd, buffer, 1024);
    printf("Request:\n%s\n", buffer);

    char *response = 
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/plain\n"
            "Content-Length: 30\n\n"
            "A monk in the cloud";
    write(client_fd, response, strlen(response));
    close(client_fd);
  }
  return 0;
}
