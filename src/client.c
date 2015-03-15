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
#include "logicdeal.h"

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

	//get the user and host
	if (client_cmdline(argv[1], &user, host)) {
		client_usage();
		return -1;
	}
	
	init_sockaddr(&address, PORT, INADDR_ANY);
	fd = create_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK);
	assert(client_connect(fd, (struct sockaddr*)&address) != -1);

	authsucceed = client_authorize(fd, user);
	if (authsucceed) {
		close(fd);
		fprintf(stdout, "user authorized failed!\n");
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
		client_printf("%s", "");
		client = (struct client_db *)malloc(sizeof(client));
		readn = getline(&buffer, &lien_len, stdin);
		if (readn == -1) continue;
		res = construct_client_db(buffer, readn, client);
		if (res) continue;	//command error, continue
		client_deal_command(fd, client);
		free(client);
		if (strcmp(buffer, "exit") == 0 ||
			strcmp(buffer, "quit") == 0)
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

//package the clone_file_to_client function as receive function
size_t client_recv_file(int fd, const char *filename)
{
	return clone_file_to_client(fd, filename);
}

//package the upload_file_from_client function as send function
size_t client_send_file(int fd, const char *filename)
{
	return upload_file_from_client(fd, filename);
}

//client receive result and client send request
size_t client_recv_result(int fd, struct server_db server)
{
	return tcp_recv_request(fd,(unsigned char *)&server,sizeof(server));
}
size_t client_send_request(int fd, struct client_db const *client)
{
	return tcp_send_result(fd,(unsigned char *)client,sizeof(*client));
}

//non-block connect with server and try 3 times if failed
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
	char *msg = "softp-wr-youngcy>type --help for more commands!\n";
	if (argc != -2) {
		fprintf(stdout, "Usage:\tsoftp Username@\"IP\"\n"
			"\t\"IP\"\n"
			"\t\"Hostname\"\n");
		exit(1);
	}

	fprintf(stdout, "%s", msg);
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
