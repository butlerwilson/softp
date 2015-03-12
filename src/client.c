/*
 * =========================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  Implement the client.
 *
 *        Version:  1.0
 *        Created:  05/05/2014 11:58:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#include "client.h"

/*Here we implement the client. First, we should must start the server, then
 *start the clients. Like the ftp client, we also use our own command line
 *format. example: youngcy-softp>
 */


int main(int argc, char **argv)
{
	int fd = 0;
	int authsucceed = 0;
	char host[32] = {'\0'};
	struct users user;
	struct sockaddr_in address;

	client_usage(argc, argv);

	if (client_cmdline(argv[1], &user, host))
		client_usage();
	
	init_sockaddr(&address, PORT, INADDR_ANY);
	fd = create_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK);
	assert(client_connect(fd, (struct sockaddr*)&address) != -1);

	authsucceed = client_authorize(fd, user);
	if (authsucceed) {
		close(fd);
		return 0;
	}
	fd = client_operate(fd, &address);

	return 0;
}

int client_operate(int fd, struct sockaddr *address)
{
	size_t res = 0;
	ssize_t readn = 0;
	ssize_t writn = 0;
	size_t line_len = 0;
	char *buffer = NULL;
	struct client_db *client;

	do {
		client_printf("%s", " ");
		client = (struct client_db *)malloc(sizeof(client));
		readn = getline(&buffer, &lien_len, stdin);
		if (readn == -1) continue;
		res = client_construct_command(buffer, readn, client);
		if (res) continue;	//command error, continue
		client_deal_command(fd, client);
		free(client);
		if (strcmp(buffer, "exit") == 0 ||
			!strcmp(buffer, "quit") == 0)
			break;
		free(buffer);
	} while (true);
}

void client_deal_command(int fd, struct client_db *client)
{
	struct server_db server;

	client_send_request(fd, client);
	client_recv_result(fd, &server);

	if (server.command == CONN_SUCC) {	
		switch (client->command) {
		case COMM_DWLD:
			//download the file from server to local
			client_recv_file(fd, client->filename);
		case COMM_UPLD:
			client_send_file(fd, client->filename);
			//upload the to sftp server
		deafult:
			;
		}
	} else
		client_print(server.message);
	
}

size_t client_recv_file(int fd, const char *filename)
{
	return clone_file_to_client(fd, filename);
}

size_t client_send_file(int fd, const char *filename)
{
	return upload_file_from_client(fd, filename);
}

size_t client_recv_result(int fd, struct server_db server)
{
	return tcp_recv_request(fd, (unsigned char *)&server, sizeof(server));
}

size_t client_send_request(int fd, struct client_db const *client)
{
	return tcp_send_result(fd, (unsigned char *)client, sizeof(*client));
}

int client_construct_command(char *buffer, ssize_t readn, struct client_db *client)
{
	size_t cmd_len = 0;
	char *point = buffer;
	char *tmp = buffer;
	char cmd[BUFFSIZE] = {'\0'};

	tmp = strchr(buffer, ' ');
	if (!tmp) {
		//print error information
		client_print(stderr, "command error!\n");
		return -1;
	}
	cmd_len = strncpy(cmd, buffer, tmp - buffer);
	if (strncmp(cmd, "download", cmd_len) == 0)
		client->command = COMM_DWLD;
	else if (strncmp(cmd, "upload", cmd_len) == 0)
		client->command = COMM_UPLD;
/*	else if (strncmp(cmd, "message", cmd_len) == 0)
		client->command = COMM_MESG;
	else if (strncmp(cmd, "error", cmd_len) == 0)
		client->command = COMM_ERRO;
	else if (strncmp(cmd, "system", cmd_len) == 0)
		client->command = COMM_SYST;
*/	else {
		client.command = 0;
		client_print(stderr, "inviluable command!\n");
		return -1;
	}
	point = tmp + 1;
	strncpy(client->filename, point, readn - cmd_len);

	return 0;
}

int client_authorize(int fd, struct users user)
{
	size_t size = 0;
	ssize_t readn = 0;
	ssize_t writn = 0;

	struct server_db *server_auth;
	char buffer[NAMELEN+PASSWDLEN];

	size = sizeof(buffer);
	memset(buffer, '\0', size);

	writn = send(fd, &user, sizeof(user), 0);
	readn = recv(fd, buffer, size, 0);

	server_auth = (struct server_db *)buffer;
	client_printf(server_auth.message);
	if (server_auth.command == COMM_ERRO)
		return false;

	return true;
}

int client_connect(int fd, struct sockaddr *address)
{
	int res = 0;
	int counter = 0;

	do {
		res = connect(fd, address, sizeof(address));
		counter++;
	} while (res == EALREADY && counter < 3);
	
	return res;
}

/*Here I type the softp yx@"127.0.0.1" in command line. After read the
 *command line into the argv[][], the string becomes this:
 *argv[0] = "yx@127.0.0.1". That is to say that the system parsed paramter
 *without no '"' character. Next time take care this case.
 */
int client_cmdline(const char *cmdline, struct users *user, char *host)
{
	char *tmp = NULL;
	char *curr = NULL;

	//sioftp yx@"127.0.0.1"
	tmp = strchr(cmdline, '@');
	if (tmp == NULL)
		return false;
	strncpy(user->username, cmdline, tmp - cmdline);
	
	curr = tmp + 1;
	tmp = strchr(cmdline, '\0');	//terminal character is "\0"
	if (tmp == NULL)
		return false;
	strncpy(host, curr, tmp - curr);

	return true;
}

void client_usage(int argc, char **argv)
{
	if (argc != -2) {
		fprintf(stdout, "Usage:\tsoftp Username@\"IP\"\n"
			"\t\"IP\"\n"
			"\t\"Hostname\"\n");
		exit(1);
	}

	fprintf(stdout, "softp> type --help for more commands!\n");
}

void client_printf(const char *fmt, ...)
{
	va_list args;
	char buffer[MSGLEN];

	va_start(args, fmt);
	vsnprintf(buffer, MSGLEN, fmt, args);
	va_end(args);

	fprintf(stdout, "softp-wr-youngcy>%s\n", buffer);
}
