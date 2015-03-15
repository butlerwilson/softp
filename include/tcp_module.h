/*
 * =========================================================================
 *
 *       Filename:  tcp_module.h
 *
 *    Description:  Define how to use tcp deal data stream.
 *
 *        Version:  1.0
 *        Created:  05/01/2014 12:15:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#ifndef _TCP_MODULE_H_
#define _TCP_MODULE_H_

#define _GUN_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "sftp.h"

/* tcp module just deal the protocol related operations, including connect,
 * bind, listen, accept, read, write. All the data has no format, they are
 * all data stream. We just read and write data stream leaving the data
 * stream to logic deal module.
 */


/*Init the socket address structure.
 *@port		port
 *@ip		ip address
 *@sockaddr	struct sockaddr_in
 *return	no return value
 */
void init_sockaddr(int port, const char *ip, struct sockaddr_in *sockaddr);

/*Create a socket file description*/
int create_socket(int domain, int type);

/*Create a tcp protocol connect
 *@argv		provide host name and port
 *return value	return listened fd
 */
int tcp_module_connect(char **argv);

/*This function deal the sockfd and recevie the data, then deal the request
 *and response to the clients.
 @sockfd:	socket fd
 @buffer:	storage the data from clients
 @size:		recevie size
 */
ssize_t tcp_recv_request(int sockfd, unsigned char *buffer, ssize_t size);

/*This function deal the sockfd and recevie the data, then deal the request
 *and response to the clients.
 @sockfd:	socket fd
 @buffer:	storage the data send to client
 @size:		recevie size
 */
ssize_t tcp_send_result(int sockfd, unsigned char *buffer, ssize_t size);

/*copy file from current host and send the buffer to client*/
ssize_t clone_file_to_client(int tcpfd, unsigned char *buffer);

/*upload file from client and write the buffer to current host*/
ssize_t upload_file_from_client(int tcpfd, unsigned char *buffer);


/*The main function of this module, it is the entry of tcp module.
 @tcpfd		file desciption
 @rset		read set pointer
 @wset		write set pointer
 return		no return value
 */
void tcp_module_deal(int tcpfd, fd_set *rset, fd_set *wset);

/*Send errors to the user*/
ssize_t tcp_response_user_error(int tcpfd, const char *errmsg);

void send_sftp_echo(int tcpfd)

/*
 *Client data structure:
 *  struct datablock {
 *	char command[COMMANDLEN];	//'0' to fill fres space
 *	char filename[FILENAMELEN];
 *	ssize_t datalen;
 *	unsigned char data[MTU - 40];
 *  };
 */
int tcp_response_client_request(ssize_t readn, unsigned char *buffer)

#endif
