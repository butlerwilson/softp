/*
 * =========================================================================
 *
 *       Filename:  sftpd.c
 *
 *    Description:  Implement the ftp tool
 *
 *        Version:  1.0
 *        Created:  04/29/2014 09:47:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */


#include <sys/socket.h>
#include <sys/types.h>
#include <apra/inet.h>
#include <sys/time.h>
#include <assert.h>

#define PROCNAME "softp"

#include "transport.h"
#include "logicdeal.h"

void system_log(int priority, const char *msg, ...)
{
	va_list args;
	char buffer[MSGLEN] = {'0'};

	va_start(args, msg);
	vsnprintf(buffer, MSGLEN, msg, args);
	va_end(args);

	openlog(PROCNAME, LOG_PID | LOG_DEALAY, LOG_USER);
	syslog(priority, buffer);
	closelog();
}

int main(int argc, char **argv)
{
	int maxfd = 0;
	fd_t fds = {-1, -1};
	fd_set rset, wret;
	struct timeval timeout;
	int clientfd = -1;

	if (argc != 2) {
		fprintf(stdout, "Usage: %s <port>\n", argv[0]);
		return 1;
	}

	fds = tp_module_connect(argv);

	if (fds.tcpfd == -1 && fds.udpfd == -1) {
		fprintf(stdout, "Fatal error, socket create failed\n ");
		return 1;
	}
	
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	maxfd = (fds.tcpfd > fds.udpfd ? fds.tcpfd : fds.udpfd) + 1;

	while (1) {
		socket_fd_set(fds.tcpfd, fds.udpfd, &rset);
		select(maxfd, &rset, NULL, NULL, &timeout);
		if (errno == EINTR)
			continue;
		else if (errno == -1) {
			fprintf(stdout, "Server-Error: select failed!\n");
			return 1;
		} else {
			fprintf(stdout, "No actions to deal...\n");
			continue;
		}
		tp_module_deal(fds, &rset, &wset);
	}

	return 0;
}
