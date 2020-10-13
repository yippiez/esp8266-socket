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
}

#include <IPAddress.h>

class TCPSocket{
    public:
        TCPSocket();

        // Basic api
        err_t connect(IPAddress ip, uint16_t port);
        err_t bind(IPAddress ip, uint16_t port);
        err_t send(const void* data, uint16_t len);
        err_t send(const void* data);
        char read();
        err_t read(char* dst);
        
        // Callbacks
        static err_t recv_callback(void* arg, tcp_pcb* pcb, pbuf* p, err_t err);
        
        // other
        err_t data_available();
    
    private:
        void _update_buffer(size_t size);
        err_t _recv(tcp_pcb* pcb, pbuf* p, err_t err);
        tcp_pcb *tcp_socket;
        uint16 _rx_pbuf_offset;
        pbuf *_rx_pbuf;

};



#endif // EasySocket_H