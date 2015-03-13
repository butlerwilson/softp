#ifndef _SFTP_DB_H_
#define _SFTP_DB_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SA		struct sockaddr
#define PORT		32766
#define TTL 		1800	//30 * 60

#define ROOTDIR		"/home/youngcy/softp/"
#define MTU		1500

#define FILENAMELEN	32

#define NAMELEN		32
#define PASSWDLEN	20
#define MSGLEN		1024

#define BUFFSIZE	64

//command types
#define COMM_DWLD	0x1	//download command
#define COMM_UPLD	0X2	//upload command
#define COMM_MESG	0X3	//send important message
#define COMM_ERRO	0x4	//send error message
#define COMM_SYST	0X5	//send system message
#define COMM_CONT	0X6	//send the continue status to client
#define COMM_TBEG	0x7	//begin transport status
#define COMM_TFIN	0X8	//finished transport status

//connect status
#define CONN_SUCC	0X1
#define CONN_REFD	0X2
#define CONN_TOUT	0X3
#define CONN_ERRO	0X4

#define USERNAME	"yx"
#define PASSWD		"123456"

typedef unsigned char uchar_t;

struct users {
	char username[NAMELEN];
	char userpasswd[PASSWDLEN];
};

struct file_properties {
	char file_name[FILENAMELEN];
	ssize_t file_size;
	ssize_t sent_size;
	ssize_t rest_size;
};

struct transport_head {
	struct file_properties prop;
	bool is_head;
	bool is_eof;
};

struct package {
	struct transport_head phead;
	uchar_t buffer[MSGLEN];
};

struct client_db {
	unsigned int command;
	char filename[FILENAMELEN];
};

/*This data structure is used for continue to transport file in server_db
 *message member item.
 *When the server_db data structure's command is COMM_CONT, the message will
 *be used as this function. and the length is the server recieved size. This
 *make the client can locate the send position or offset size.
 *When use this data structure, the server_db will like this:
 *	struct server_db {
 *	  command = COMM_CONT;
 *	  length = xx;
 *	  message[] = "123xxx.tx";
 *	}
 */
struct file_info {
	size_t	fn_length;	//file name length
	size_t	file_size;	//file total size
	size_t	rest_size;	//rest file size
	uchar_t	*file_namE;	//filename
};

struct server_db {
	unsigned int command;
	size_t length;
	uchar_t message[MSGLEN];
};

#endif
