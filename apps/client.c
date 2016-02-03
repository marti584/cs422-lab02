/* echoclient.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cnaiapi.h>
#include <netinet/in.h>

#define BUFFSIZE		1000000

int readln(char *, int);

/*-----------------------------------------------------------------------
 *
 * Program: echoclient
 * Purpose: contact echoserver, send user input and print server response
 * Usage:   echoclient <compname> [appnum]
 * Note:    Appnum is optional. If not specified the standard echo appnum
 *          (7) is used.
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	computer	comp;
	appnum		app;
	connection	conn;
	char		buff[BUFFSIZE];
	unsigned long			len;

	if (argc < 2 || argc > 3) {
		(void) fprintf(stderr, "usage: %s <SERVER_IP> [SERVER_APPLICATION_NUMBER]\n", argv[0]);
		exit(1);
	}

	/* convert the arguments to binary format comp and appnum */

	comp = cname_to_comp(argv[1]);
	if (comp == -1)
		exit(1);

	if (argc == 3)
		app = (appnum) atoi(argv[2]);
	else
		if ((app = appname_to_appnum("echo")) == -1)
			exit(1);
	
	/* form a connection with the server */

	conn = make_contact(comp, app);
	if (conn < 0) 
		exit(1);

	/* iterate: read input from the user, send to the server,	*/
	/*	    receive reply from the server, and display for user */

	while ((len = readln(buff, BUFFSIZE)) > 0) {

		/* Read every line and check if it starts with a tab */
		if (buff[0] == '\t') {
			
			char line[BUFFSIZE];
			unsigned long len2;

			/* Now we can read the input and store it in a temp array
				of characters and concatenate the buffer so we can get
				our whole paragraph */
			while ((readln(line, BUFFSIZE) > 0) && (line[0] != '\n')) {
				(void) strcat(buff, line);
				len2 = strlen(line);
				len += len2;
				memset(&line[0], 0, sizeof(line));
			}
			uint32_t length = htonl(len);

			send(conn, &length, 4, 0);

			if ((len2 = send(conn, buff, len, 0)) < 0) {
				printf("Send Failed");
				fflush(stdout);
			}
			memset(&buff[0], 0, sizeof(buff));
			memset(&line[0], 0, sizeof(line));
			fflush(stdout);
		}
	}

	/* iteration ends when EOF found on stdin */

	(void) send_eof(conn);
	(void) printf("\n");
	return 0;
}
