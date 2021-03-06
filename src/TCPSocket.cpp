
extern "C"{
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
#include "osapi.h"
#include "string.h"
}

#include "EasySocket.h"
#include <IPAddress.h>

TCPSocket::TCPSocket(){
    tcp_socket = tcp_new();
    tcp_recv(tcp_socket, &recv_callback);
    tcp_arg(tcp_socket, this);  // arg field in callback functions becames this object
}

TCPSocket::TCPSocket(tcp_pcb* pcb){
    tcp_socket = pcb;
    tcp_recv(tcp_socket, &recv_callback);
    tcp_arg(tcp_socket, this);
}

err_t TCPSocket::connect(IPAddress ip, uint16_t port){
    if (!tcp_socket)
        return ERR_CONN;

    err_t err = tcp_connect(tcp_socket, ip, port, NULL);
    return err;
}

err_t TCPSocket::bind(IPAddress ip, uint16_t port){
    if (!tcp_socket)
        return ERR_CONN;

    err_t err = tcp_bind(tcp_socket, ip, port);

    return err;
}

void TCPSocket::listen(){
    tcp_socket = tcp_listen(tcp_socket);  // listen destroys the given and returns more minimal pcb
    tcp_accept(tcp_socket, &accept_callback);
    tcp_arg(tcp_socket, this);

    tcp_socket->so_options |= SOF_REUSEADDR;

    is_listening = true;
}

TCPSocket* TCPSocket::accept(){
    
    if(!_list_head){
        return 0;
    }

    tcp_pcb *pcb = _list_head->self->tcp;
    TCPSocket *new_soc = new TCPSocket(pcb);
    
    if(!new_soc){
        return 0;
    }

    if(pcb)
        tcp_backlog_accepted(pcb); // allow lwip to have one more connection

    _list_head = _list_head->next;

    return new_soc;
}

err_t TCPSocket::_accept(tcp_pcb* newpcb, err_t err){
    // TODO: Check for memory errors
    pcb_node *new_node;
    new_node = (pcb_node *)malloc(sizeof(pcb_node));

    if(!new_node){
        return ERR_MEM;
    }

    new_node->self = (pcb_types *)malloc(sizeof(pcb_types));
    new_node->self->tcp = newpcb;
    new_node->next = NULL;

    tcp_backlog_delayed(newpcb);

    _append_node(new_node);
    return err;
}

err_t TCPSocket::accept_callback(void* arg, tcp_pcb* pcb, err_t err){
    return reinterpret_cast<TCPSocket *>(arg)->_accept(pcb, err);  // arg is the class instance set by tcp_arg
}

err_t TCPSocket::send(const void* data, uint16_t len){
    if(!tcp_socket)
        return ERR_CONN;

    size_t max_size = tcp_sndbuf(tcp_socket);
    if(len > max_size) len = max_size;  // for now nothing more than what pcb allows can be sent
    tcp_write(tcp_socket, data, len, 0);  // writes into buffer

    return tcp_output(tcp_socket);  // sends buffer out
}

err_t TCPSocket::send(const void* data){
    return TCPSocket::send(data, strlen((const char *)data));
}

err_t TCPSocket::_recv(tcp_pcb* pcb, pbuf* p, err_t err){
    // Check for offset
    (void) pcb;
    
    if(p==NULL){
        // connection closed
        conn_alive = false;

        if(_rx_pbuf){
            return ERR_OK;
        }else{
            abort();
            return ERR_ABRT;
        }
    }

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

err_t TCPSocket::read(char* dst){
    if(!_rx_pbuf)
        return ERR_BUF;
    
    os_memcpy(dst, ((char *)(_rx_pbuf->payload)), _rx_pbuf->len);

    _update_buffer(_rx_pbuf->len);
    
    return ERR_OK;
}

err_t TCPSocket::recv_callback(void* arg, tcp_pcb* pcb, pbuf* p, err_t err){
    return reinterpret_cast<TCPSocket*>(arg)->_recv(pcb, p, err);
}

err_t TCPSocket::data_available(){
    return _rx_pbuf ? 1 : 0;
}

err_t TCPSocket::client_available(){
    return _list_head ? 1 : 0;
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

// PCB LINKED LIST SECTION
void TCPSocket::_append_node(pcb_node *new_node){
    if(!_list_head){
        _list_head = new_node;
    }else{
        pcb_node *tmp;
        tmp = _list_head;
        while(tmp){
            tmp = tmp->next;
        }
        tmp->next = new_node;
    }
}

TCPSocket::operator bool(){
    return tcp_socket && conn_alive;
}

err_t TCPSocket::abort(){

    // TODO: test if this actualy needed
    if(is_listening){
        tcp_accept(tcp_socket, NULL);
    }

    if(tcp_socket){
        tcp_arg(tcp_socket, NULL);
        tcp_recv(tcp_socket, NULL);
        tcp_abort(tcp_socket);
        tcp_socket = NULL;
    }
    
    return ERR_ABRT;
}