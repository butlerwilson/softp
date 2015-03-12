#ifndef _LOGICDEAL_H_
#define _LOGICDEAL_H_

/*Client authority data structure:
 *  struct user {
 *	char username[32];
 *	char *userpasswd[20];
 *  };
 */
int user_authorize_request(const unsigned char * const buffer, struct users *user);

/*Construct the server_db structure.
 @cmd		command
 @buffer	value
 @size		size
 @msg		message
 return		no value
 */
void construct_server_db(int cmd, char *buffer, size_t size, struct server_db *msg);

/*server authorize the clients and response the authorise result.
 @auth		the server_db pointer
 */
int server_authorize_result(struct server_db *auth);

#endif
