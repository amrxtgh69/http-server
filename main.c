#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void send_file(int client_fd, const char *filename, const char *content_type) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    char *not_found = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n\r\n"
        "404 Not Found";
    write(client_fd, not_found, strlen(not_found));
    return;
  }
  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  rewind(file);

  char *file_content = malloc(filesize);
  fread(file_content, 1, filesize, file);
  fclose(file);

  //response header 
  char header[256];
  snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n\r\n",
             content_type, filesize);
  write(client_fd, header, strlen(header));
  write(client_fd, file_content, filesize);

  free(file_content);
}


int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[2048] = {0};
  
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  bind(server_fd, (struct sockaddr*)&address, sizeof(address));

  listen(server_fd, 3);
  printf("Server listening on PORT: %d\n", PORT);

  while(1) {
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    int valread = read(client_fd, buffer, sizeof(buffer) - 1);
    buffer[valread] = '\0';
    printf("Request:\n%s\n", buffer);

    if (strncmp(buffer, "GET /index", 10) == 0 || strncmp(buffer, "GET /index.html", 15) == 0) {
      send_file(client_fd, "index.html", "text/html");
    }
    else if (strncmp(buffer, "GET /favicon.ico", 16) == 0) {
      send_file(client_fd, "favicon.ico", "image/x-icon");
    }
    else {
      char *response = 
         "HTTP/1.1 404 Not Found\r\n"
         "Content-Type: text/plain\r\n"
         "Content-Length: 13\r\n\r\n"
         "404 Not Found";
      write(client_fd, response, strlen(response));
    }
    close(client_fd);
  }
  return 0;
}
