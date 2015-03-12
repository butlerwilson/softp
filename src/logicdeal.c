#include <string.h>

#include "logicdeal.h"

//include the db.h which include the basic data structure
#include "db.h"

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


/*server authorize the clients and response the authorise result.
 @auth		the server_db pointer
 */
int server_authorize_result(struct server_db *auth)
{
	construct_server_db(CONN_SUCC,"Server authorized succeed!",
				MSGLEN, auth);

	return true;
}
