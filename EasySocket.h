#ifndef EasySocket_H
#define EasySocket_H

#define __LWIP_TCP_H__ 

// Without this ESP8266Wifi libary causes an error because wl_tcp_state is redefined in wl_definitions.h
// https://github.com/esp8266/Arduino/issues/7249

// INADDR_ANY is used for any ip addres 0.0.0.0
#define STRLEN(x) (sizeof(x)-1)


extern "C"{
#include "lwip/tcp.h"
}

#include <IPAddress.h>

class TCPSocket{
    public:
        TCPSocket();
        err_t connect(IPAddress ip, uint16_t port);
        err_t send(const void* data, uint16_t len);
        err_t send(const void* data);
        //err_t recv(void *arg, tcp_pcb* pcb, pbuf* p, err_t err);
    
    private:
        tcp_pcb *tcp_socket;
};

#endif // EasySocket_H