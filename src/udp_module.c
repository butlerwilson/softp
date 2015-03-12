/*
 * =========================================================================
 *
 *       Filename:  udp_module.c
 *
 *    Description:  Implement the udp operations.
 *
 *        Version:  1.0
 *        Created:  05/01/2014 12:23:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#include "udp_module.h"
#include "sftp.h"

int udp_module_connect(char **argv)
{
	int udpfd = 0;
	int success = 0;
	struct sockaddr_in udpsockaddr;

	init_server_data(argv, &udpsockaddr);
	udpfd = create_socket(AF_INET, SOCK_DGRAM);
	if (udpfd == -1) {
		fprintf(stdout, "UDP-Warning: create udp socket failed!\n");
		return -1;
	}

	success = bind(udpfd, (SA *)&udpsockaddr, sizeof(SA));
	if (success) {
		fprintf(stdout, "UDP-Error: udp bind failed\n");
		return -1;
	}
}
