#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

char *getpage(char *filename, char *headers) {
    int chr;
    size_t buffer_size = 1024;
    size_t length = 0;

    char *payload = malloc(buffer_size);

    FILE *page = fopen(filename, "r");

    while ((chr = fgetc(page)) != EOF) {
        if (length + 2 > buffer_size) {
            buffer_size *= 2;
            payload = realloc(payload, buffer_size);
        }
        payload[length++] = chr;
    }
    payload[length] = '\0';

    fclose(page);

    size_t total_length = strlen(headers) + strlen(payload) + 1;
    char *response = malloc(total_length);

    strcpy(response, headers);
    strcat(response, payload);

    free(payload);

    return response;
}

int main(int argc, char *argv[]) {
    int serversock;
    serversock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    const int enable = 1;
    setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    int binding = bind(serversock, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    int listening = listen(serversock, 8);

    printf("[+] Now listening (%d|%d)\n", binding, listening);

    char request[2048];
    char *response;

    char req_method[32];
    char req_path[32];
    char req_version[32];

    int clientsock;
    while (1) {
        clientsock = accept(serversock, NULL, NULL);

        recv(clientsock, request, sizeof(request), 0);
        sscanf(request, "%s %s %s\n", req_method, req_path, req_version);
        printf("%s %s %s\n", req_method, req_path, req_version);

        if (strcmp(req_path, "/") == 0) {
            response = getpage(
                "./assets/index.html",
                "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
            );
        } else if (strcmp(req_path, "/style.css") == 0) {
            response = getpage(
                "./assets/style.css",
                "HTTP/1.1 200 OK\nContent-Type: text/css\n\n"
            );
        } else {
            response = getpage(
                "./assets/404.html",
                "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n"
            );
        }
        send(clientsock, response, strlen(response), 0);
        
        close(clientsock);

        memset(response, 0, strlen(response));
    }

    close(serversock);

    return 0;
}