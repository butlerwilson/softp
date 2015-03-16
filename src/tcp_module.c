/*
 * =========================================================================
 *
 *       Filename:  tcp_module.c
 *
 *    Description:  Implement the basic tcp module operate functions
 *
 *        Version:  1.0
 *        Created:  05/01/2014 12:07:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */
#include <string.h>

#include "tcp_module.h"
#include "logicdeal.h"

void init_sockaddr(short port,const char * ip,struct sockaddr_in *sockaddr)
{
	sockaddr->sa_family = AF_INET;
	sockaddr->sa_port = port;
	sockaddr->sa_addr = htonl(ip);
}

int create_socket(int domain, int type)
{
	int fd = 0;
	char errmsg[1024] = {'0'};

	fd = socket(domain, type, 0);

	if (fd == -1) {
		if (type & SOCK_DGRAM)
			strncpy(errmsg, "Warning: udp unavalible\n", 64);
		else
			strncpy(errmsg, "Warning: tcp unavalible\n", 64);
		system_log(LOG_WARNING, errmsg);
		fprintf(stderr, "sftp-Warning: %s\n", errmsg);
	}

	return fd;
}

int tcp_module_connect(char **argv)
{
	int tcpfd = 0;
	short port = 0;
	int success = 0;
	struct sockaddr_in tcpsockaddr;
	
	port = atoi(argv[1]);
	if (port == -1) {
		system_log(LOG_ERR, "sftp-Error: port illegal\n");
		fprintf(stdout, "Error: %s port illegal\n", argv[0]);
		exit(1);
	}

	init_sockaddr(PORT, INADDR_ANY, &tcpdockaddr);
	tcpfd = create_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK);
	if (tcpfd == -1) {

		system_log(LOG_WARNING, "TCP-Warning: create tcp socket failed!\n");
		return -1;
	}

	success = setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR, 1, 4);
	if (success)
		system_log(LOG_USER, "Note: tcp set socket option failed!\n");
	success = bind(tcpfd, (SA *)&tcpsockaddr, sizeof(SA));
	if (success) {
		system_log(LOG_ERR, "TCP-Error: tcp bind failed!\n");
		return -1;
	}

	success = listen(tcpfd, 5);
	if (success) {
		system_log(LOG_WARNING, "TCP-Warning: Current connection in susing\n");
		sleep(2);
	}

	return tcpfd;
}

void tcp_module_deal(int tcpfd, fd_set *rset, fd_set *wset)
{
	int addrlen = 0;
	int clientfd = 0;
	time_t success = 0;
	struct sockaddr clientaddr;

	if (FD_ISSET(tcpfd, rset)) {	//has client ready to connect
		clientfd = accept(tcpfd, &clientaddr, &addrlen);
		if (clientfd != -1) {
			tcp_accept_client_conn(clientfd);
			shutdown(clientfd, SHUT_RDWR);
		}
	}
}

/*This function deal the sockfd and recevie the data, then deal the request
 *and response to the clients.
 @sockfd:	socket fd
 @buffer:	storage the data from clients
 @size:		recevie size
 */
ssize_t tcp_recv_request(int sockfd, unsigned char *buffer, ssize_t size)
{
	ssize_t count = 0;
	ssize_t readn = 0;

	do {
		readn = recv(clientfd, buffer + readn, size - readn, 0);
		count += readn;
	} while (readn > 0);

	return count;
}

/*This function deal the sockfd and recevie the data, then deal the request
 *and response to the clients.
 @sockfd:	socket fd
 @buffer:	storage the data send to client
 @size:		recevie size
 */
ssize_t tcp_send_result(int sockfd, unsigned char *buffer, ssize_t size)
{
	ssize_t count = 0;
	ssize_t writn = 0;

	do {
		writn = send(sockfd, buffer + writn, size - writn, 0);
		count += writn;
	} while (writn > 0);

	return count;
}

ssize_t clone_file_to_client(int tcpfd, const char *filename)
{
	int fd = 0;
	struct stat st;
	ssize_t size = 0;
	ssize_t sendn = 0;
	struct server_db db;

	fd = open(filename, O_RDONLY);
	if ((fd != -1) && !stat(fd, &st))
		size = st.st_size;
	else
		return false;

	memset(db, '\0', sizeof(db));
	db.command = COMM_DWLD;
	db.length = size;

	tcp_send_result(tcpfd, (unsigned char *)&db, sizeof(db));
	sendn = sendfile(tcpfd, fd, NULL, size);
	close(fd);

	return sendn == size >= 0 ? size : -1;
}

ssize_t upload_file_from_client(int tcpfd, const char *filename)
{
	struct datablock *tmp = NULL;

	int fd = 0;
	int res = 0;
	int pipefd[2];
	size_t offset = 0;
	ssize_t writn = 0;

	fd = open(filename, O_RDWR | O_APPEN);
	res = pipe2(pipefd, O_NONBLOCK);
	if (res == -1 || fd == -1) {
		system_log(LOG_WARNING, "Create file: %s failed!\n",
				filename);
		tcp_response_user_error(tcpfd, "Create file: %s failed!\n",
				filename);
		return -1;
	}

	do {
		//read data from tcpfd and write to pipefd[1]
		res = splice(tcpfd, NULL, pipefd[1], NULL, 
				0, SPLICE_NONBLOCK);
		//read data from pipefd[0] and write to fd
		writn += splice(pipefd[0], NULL, fd, NULL, res);
	} while (res >= 0);

	return writn;
}

int tcp_accept_client_conn(int clientfd)
{
	int success = 0;
	int authsuccess;
	ssize_t readn = 0;
	ssize_t writn = 0;
	struct users user;
	unsigned char tc = 8;
	unsigned char buffer[BUFFSIZE];

	//recevie the user authorize
	readn = tcp_recv_request(clientfd, buffer, sizeof(user));
	//authorize user if legal
	authsuccess = user_authorize_request(buffer, &user);
	if (authsuccess == false) {
		system_log(LOG_WARNING, "Server refused user: %s\n",
				user.username);
		tcp_response_user_error(clientfd,"Server refused serve!\n");
		return 1;	//next step close connection
	} else {	//authorize succeed, send succeed flags to clients
		struct server_db auth;
		server_authorize_result(&auth);
		tcp_send_result(clientfd, (unsigned char*)&auth, sizeof(auth));
	}

	success = time(&time_start);
	if (success = -1) {
		system_log(LOG_WARNING, "Server-warning: Got time failed!\n");
	}
	time_ended = time_start + TTL;
	//after authorized succeed, recevie user request
	memset(buffer, '\0', BUFFSIZE);
	chdir(ROOTDIR);
	do {
		//per loop 3 times, check if time is out
		if (tc = tc >> 1, tc == 0 && tc = 8 &&
			!tcp_connected_time_out(time_end)) {
			system_log(LOG_USER, "Connected time out! Shutdown now!\n");
			tcp_response_user_error(clientfd,
				"Connected time out! reconnect manualy.\n");
			return 1;
		}

		readn = tcp_recv_request(clientfd, buffer,
						sizeof(struct users));
		tcp_response_client_requset(clientfd, readn, buffer);
	} while (readn > 0);

	return 0;
}

int tcp_connected_time_out(time_t time_end)
{
	int success = 0;
	time_t curr_time;

	success = time(&curr_time);
	if (success = -1) {
		system_log(LOG_WARNING, "Server-warning: Got time failed!\n");
	}
	if (curr_time == time_end)
		return 0;
	return 1;
}

int tcp_response_user_error(int clientfd, const char *errmsg, ...)
{
	va_list args;
	struct server_db msg;
	char buffer[BUFFSIZE] = {'0'};

	va_start(args, errmsg);
	vsnprintf(buffer, BUFFSIZE, errmsg, args);
	va_end(args);

	construct_server_db(CONN_ERRO, buffer, BUFFSIZE, &msg);
	tcp_send_result(clientfd, (unsigned char *)msg, sizeof(msg));
}

void send_sftp_echo(int clientfd)
{
	char sftp_echo[MSGLEN];
	ssize_t size = sizeof(sftp_echo);

	snprintf(sftp_echo, size, "%s", "sftp<write to youngcy>:");
	send_server_result(clientfd, (unsigned char *)sftp_echo, 1, NULL);
}

/*
 *Client data structure:
 *  struct datablock {
 *	char command[COMMANDLEN];	//'0' to fill fres space
 *	char filename[FILENAMELEN];
 *	ssize_t datalen;
 *	unsigned char data[MTU - 40];
 *  };
 */
int tcp_response_client_request(int clientfd, ssize_t readn, unsigned char *buffer)
{
	int success = 0;
	size_t offset = 0;
	ssize_t datalen = 0;
	char message[BUFFSIZE] = {'\0'};
	char *filename = ((struct users *)buffer)->filename;

	//send the COMM_BEG command to clients.
	struct server_db response = {COMM_TBEG, MSGLEN, "begin transport."};
	server_send_result(clientfd, response, MSGLEN);

	switch (command) {
	case COMM_DWLD:
		success = clone_file_to_client(clientfd, filename);
		break;
	case COMM_UPLD: 
		success = upload_file_from_client(clientfd, filename);
		break;
	case COMM_SYST:
		break;
	default:
		tcp_response_user_error(clientfd, "Unknown command!\n");
	}

	if (success != true) {
		snprintf(message, sizeof(buffer), strerror(success));
		tcp_response_user_error(clientfd, message);
	}

	return success;
}
