
extern "C"{
#include "lwip/tcp.h"
}

#include "EasySocket.h"
#include <IPAddress.h>

TCPSocket::TCPSocket(){
    tcp_socket = tcp_new();
}

err_t TCPSocket::connect(IPAddress ip, uint16_t port){
    if (!tcp_socket)
        return ERR_CONN;

    err_t err = tcp_connect(tcp_socket, ip, port, NULL);
    return err;
}

err_t TCPSocket::send(const void* data, uint16_t len){
    if(!tcp_socket)
        return ERR_CONN;

    size_t max_size = tcp_sndbuf(tcp_socket);
    if(len > max_size) len = max_size;  // for now nothing more than what pcb allows can be sent
    tcp_write(tcp_socket, data, len, 0);

    return tcp_output(tcp_socket);
}

err_t TCPSocket::send(const void* data){
    return TCPSocket::send(data, STRLEN(data));
}