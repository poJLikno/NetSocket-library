#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

int main() {
    int _listen_socket = -1;
    sockaddr_in _socket_info = { 0 };
    
    int _conn_socket = -1;
    sockaddr_in _conn_info = { 0 };
    socklen_t _conn_info_size = sizeof(_conn_info);
    
    char buf[1024];
    ssize_t bytes_read;
    
    _socket_info.sin_family = AF_INET;
    _socket_info.sin_port = htons(SERVER_PORT);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);// Or another IP
    
    if ((_listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }
    
    if (bind(_listen_socket, (const sockaddr *)&_socket_info, sizeof(_socket_info)) < 0) {
        perror("bind");
        exit(2);
    }
    
    if (listen(_listen_socket, SOMAXCONN) < 0) {
        perror("listen");
        exit(3);
    }
    
    printf("Listening...\n");
    
    if ((_conn_socket = accept(_listen_socket, (sockaddr *)&_conn_info, (socklen_t *)&_conn_info_size)) < 0) {
        perror("accept");
        exit(4);
    }
    
    if ((bytes_read = recv(_conn_socket, buf, 1024, 0))) {
        printf("%s\n", buf);
        send(_conn_socket, buf, bytes_read, 0);
    }
    
    if (shutdown(_conn_socket, 1/*SD_SEND*/) < 0) {
        perror("shutdown");
        exit(5);
    }
    
    close(_listen_socket);
    close(_conn_socket);
    
    return 0;
}
