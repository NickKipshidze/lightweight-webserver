#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    char *address;
    char *request;
    int port;

    if (argc < 2) {
        printf("[-] Not enough arguments provided;\n");
        printf("[?] Example: %s <ip> [port] [request]\n", argv[0]);
        return -1;
    } else {
        printf("[+] Setting target address: %s\n", argv[1]);
        address = argv[1];
    }
    
    if (argc < 3) {
        printf("[-] Port number not provided (argument 2);\n");
        printf("[+] Using default port number 80;\n");
        port = 80;
    } else {
        printf("[+] Setting target port: %s\n", argv[2]);
        port = atoi(argv[2]);
    }

    if (argc < 4) {
        printf("[?] You can also specify your request string (argument 3);\n");
        printf("[+] Using default: \"GET / HTTP/1.1\"\n");
        request = "GET / HTTP/1.1\r\n\r\n";
    } else {
        printf("[+] Setting request string;\n");
        request = argv[3];
    }

    int clientsock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in remoteaddr;
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(port);
    inet_aton(address, (struct in_addr *) &remoteaddr.sin_addr.s_addr);

    int connection = connect(clientsock, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr));

    printf("[+] Connection: %d\n", connection);

    char response[524288];
    char data[524288];

    send(clientsock, request, strlen(request), 0);
    printf("[+] Request sent;\n");

    int received;
    while ((received = recv(clientsock, response, sizeof(response)-1, 0)) > 0) {
        strcat(data, response);
        memset(response, 0, sizeof(response));
        if (strlen(data) >= 16318)
            break;
    }

    printf("\n[+] Response:\n%s\n", data);

    close(clientsock);

    return 0;
}