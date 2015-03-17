#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

//include tcp protocol related operations
#include "tcp_module.h"

//include udp protocol related operations
#include "udp_module.h"

/*This file mainly include some transport functions, all of them are the
 *basic function.
 *  tp_module_connect()
 *  tp_module_deal()
 */

typedef struct fd_sets {
	int tcpfd;
	int udpfd;
}fd_t;

/*add tcpfd and udpfd into the read fd_set*/
void socket_fd_set(int tcpfd, int udpfd, fd_set *rset);

/*begin the tcp and udp connect*/
fd_t tp_module_connect(char **argv);

/*deal the data base on the tcp and udp*/
void tp_module_deal(fd_t fds, fd_set *rset, fd_set *wset);

#endif
