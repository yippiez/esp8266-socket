// Linked list for pcb
#ifndef LINKEDPCB_H
#define LINKEDPCB_H

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/raw.h"

/*
typedef enum {
    //No errors
    LINK_ERR_OK = 1
} link_err_t;
*/

typedef union pcb_types{
    tcp_pcb* tcp;
    udp_pcb* udp;
    raw_pcb* raw;
} pcb_types;

typedef struct pcb_node{
    union pcb_types *self;
    struct pcb_node *next;
} pcb_node;

#endif // LINKEDPCB_H