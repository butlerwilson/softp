/*
 * =========================================================================
 *
 *       Filename:  client.h
 *
 *    Description:  Define what will the clients will do
 *
 *        Version:  1.0
 *        Created:  05/12/2014 11:58:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "db.h"
#include "tcp_module.h"

/*we define this macro in sftp.h, undefine here and redefine it.*/
#undef PROCNAME
#define PROCNAME "softp_client"
#undef false
#define false	1
#undef true
#define true	0

#undef USERNAME
#define USERNAME	"yx"
#define PASSWD		"123456"

//if error, print the usage.
void client_usage(int argc, char **argv);

/*Connect to the server.
 *@fd		//created by socket
 *@address	//server address information
 *return value	//succeed zero returns 
 */
int client_connect(int fd, struct sockaddr *address);

/*Parse the client command line.
 *@cmdline	//command line string
 *@username	//get user name from cmdline
 *@host		//get host from the command line
 *return value	//succeed true returned
 */
int client_cmdline(const char *cmdline, char *username, char *host);

/*client authorize
 *@fd		//connected file descriptor
 *return value	//succeed true returned
 */
int client_authorize(int fd);

/*base the user command to operate the files
 *@fd		//connected file descriptor
 *@address
 *return	//fd
 */
int client_opearte(int fd, struct sockaddr *address);

/*construct the client command
 *buffer	//client input chacrators
 *readn		//client input line length
 *client	//client_db structure
 *return value	//no return value
 */
void client_construct_command(char *buffer, ssize_t readn, struct client_db *client);

/*base the input and send message to server
 *@fd		//connected file descriptor
 *@client	//client command data
 *return value	//return sent size
 */
size_t client_send_msg_to_server(int fd, struct client_db client);

//client print the information
void client_print(const char *fmti, ...);

#endif
