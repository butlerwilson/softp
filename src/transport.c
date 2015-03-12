#include "transport.h"

void socket_fd_set(int tcpfd, int udpfd, fd_set *rset)
{
	if (tcpfd != -1)
		FD_SET(tcpfd, rset);
	if (udpfd != -1)
		FD_SET(udpfd, rset);
}

fd_t tp_module_connect(char **argv)
{
	fd_t fds = {-1, -1};

	fds.tcpfd = tcp_module_connect(argv);
	fds.udpfd = udp_module_connect(argv);

	return fds;
}


void tp_module_deal(fd_t fds, fd_set *rset, fd_set *wset)
{
	tcp_module_deal(fds.tcpfd, rset, wset);
	udp_module_deal(fds.udpfd, rset, wset);
}
