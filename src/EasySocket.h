#ifndef EasySocket_H
#define EasySocket_H

#define __LWIP_TCP_H__ 

// Without this ESP8266Wifi libary causes an error because wl_tcp_state is redefined in wl_definitions.h
// https://github.com/esp8266/Arduino/issues/7249

// INADDR_ANY is used for any ip addres 0.0.0.0

extern "C"{
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
#include "linkedpcb.h"
}

#include <IPAddress.h>

class TCPSocket{
    public:
        TCPSocket();
        TCPSocket(tcp_pcb* pcb);
        // Basic api
        err_t connect(IPAddress ip, uint16_t port);
        err_t bind(IPAddress ip, uint16_t port);
        void listen();
        TCPSocket* accept();
        err_t send(const void* data, uint16_t len);
        err_t send(const void* data);
        char read();
        err_t read(char* dst);
        err_t abort();
        
        // Callbacks
        static err_t recv_callback(void* arg, tcp_pcb* pcb, pbuf* p, err_t err);
        static err_t accept_callback(void* arg, tcp_pcb* pcb, err_t err);

        // other
        err_t data_available();
        err_t client_available();
        operator bool();
        bool conn_alive = true;
        bool is_listening = false; // TODO: check if this needed
        // bool debug_flag = false;

    private:
        void _update_buffer(size_t size);
        err_t _recv(tcp_pcb* pcb, pbuf* p, err_t err);
        err_t _accept(tcp_pcb* pcb, err_t err);

        //linked list
        void _append_node(pcb_node *new_node);

        // variables
        tcp_pcb *tcp_socket = NULL;
        uint16 _rx_pbuf_offset = 0;
        pbuf *_rx_pbuf = NULL;
        pcb_node *_list_head = NULL;  // head of the pcb linked list
        pcb_node *_list_tail = NULL;  // tail of the pcb linked list

};



#endif // EasySocket_H