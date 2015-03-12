#include <string.h>

#include "logicdeal.h"

//include the db.h which include the basic data structure
#include "db.h"

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
	if (server_auth.command == COMM_ERR)
		return false;

	return true;
}


/*Client authority data structure:
 *  struct user {
 *	char username[32];
 *	char *userpasswd[20];
 *  };
 */
int user_authorize_request(unsigned char * const buffer, struct users *user)
{
	int nameok = 0;
	int passwdok = 0;		
	struct users *tmp = NULL;

	tmp = (struct users *)buffer;
	nameok = strcmp(tmp->username, USERNAME);
	passwdok = strcmp(tmp->userpasswd, PASSWD);

	strncpy(user->username, tmp->username, sizeof(user->username));	
	if (nameok == 0 && passwdok == 0)
		return true;
	return false;
}

/*Construct the server_db structure.
 @cmd		command
 @buffer	value
 @size		size
 @msg		message
 return		no value
 */
void construct_server_db(int cmd, char *buffer, size_t size, struct server_db *msg)
{
	memset(msg, '\0', size);
	msg.command = cmd;
	msg.length = size;
	strncpy(msg.message, buffer, size);
}

//construct the client command when user input the command
int construct_client_db(char *buffer, ssize_t readn, struct client_db *client)
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
	else if (strncmp(cmd, "message", cmd_len) == 0)
		client->command = COMM_MESG;
	else if (strncmp(cmd, "error", cmd_len) == 0)
		client->command = COMM_ERRO;
	else if (strncmp(cmd, "system", cmd_len) == 0)
		client->command = COMM_SYST;
	else {
		client.command = 0;
		client_print(stderr, "inviluable command!\n");
		return -1;
	}
	point = tmp + 1;
	strncpy(client->filename, point, readn - cmd_len);

	return 0;
}

/*server authorize the clients and response the authorise result.
 @auth		the server_db pointer
 */
int server_authorize_result(struct server_db *auth)
{
	construct_server_db(CONN_SUCC,"Server authorized succeed!",
				MSGLEN, auth);

	return true;
}
