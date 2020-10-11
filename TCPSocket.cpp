/*
TODO
    Read x bytes out of buffer
    bind/listen
*/
extern "C"{
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
}

#include "EasySocket.h"
#include <IPAddress.h>

TCPSocket::TCPSocket(){
    tcp_socket = tcp_new();
    tcp_recv(tcp_socket, &recv_callback);
    tcp_arg(tcp_socket, this);  // arg field in callback functions becames this object
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

err_t TCPSocket::_recv(tcp_pcb* pcb, pbuf* p, err_t err){
    // Check for offset
    (void) pcb;
    
    if (!_rx_pbuf){    
        _rx_pbuf = p;
        _rx_pbuf_offset = 0;
    }
    else{
        pbuf_cat(_rx_pbuf, p);
    }
    
    return err;
}

char TCPSocket::read(){
    if(!_rx_pbuf)
        return 0;

    char c = ((char *)(_rx_pbuf->payload))[_rx_pbuf_offset];
    _update_buffer(1);
    return c;
}

char TCPSocket::peek(){
    if(!_rx_pbuf)
        return 0;

    return ((char *)(_rx_pbuf->payload))[_rx_pbuf_offset];
}

err_t TCPSocket::recv_callback(void* arg, tcp_pcb* pcb, pbuf* p, err_t err){
    return reinterpret_cast<TCPSocket*>(arg)->_recv(pcb, p, err);
}

void TCPSocket::_update_buffer(size_t size){
    if(tcp_socket)
        tcp_recved(tcp_socket, size);
    
    ptrdiff_t left = _rx_pbuf->len - _rx_pbuf_offset - size;
    if(left > 0) { // if pointer hasn't reached end of pbuf
        _rx_pbuf_offset += size;
    } else if(!_rx_pbuf->next) { // if its end of pbuf and there is no next pbuf to read
        pbuf_free(_rx_pbuf);
        _rx_pbuf = 0;
        _rx_pbuf_offset = 0;
    }else{ // if its end of pbuf and there is a next pbuf to read
        auto temp = _rx_pbuf;
        _rx_pbuf_offset = 0;
        _rx_pbuf = _rx_pbuf->next;
        pbuf_ref(_rx_pbuf);
        pbuf_free(temp);
    }
}