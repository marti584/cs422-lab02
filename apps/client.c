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
	int			len;

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

	// (void) printf(INPUT_PROMPT);
	// (void) fflush(stdout);

	int lastMessageSent = 1;

	/* iterate: read input from the user, send to the server,	*/
	/*	    receive reply from the server, and display for user */

	while ((len = readln(buff, BUFFSIZE)) > 0) {

		/* Read every line and check if it starts with a tab */
		if (buff[0] == '\t') {

			char line[BUFFSIZE];
			int len2;
			lastMessageSent = 0;

			/* Now we can read the input and store it in a temp array
				of characters and concatenate the buffer so we can get
				our whole paragraph */
			while ((fgets(line, BUFFSIZE, stdin) != NULL) && (line[0] != '\n')) {
				(void) strcat(buff, line);
				len2 = strlen(line);
				len += len2;
				// printf("Packets Sent: %d", len2);
				// fflush(stdout);
			}
			uint32_t length = htonl(len);
			// if (send(conn, length, 4, 0) < 0)
			// 	printf("Send Failed");
			printf("%d\n", length);

			char paragraph[BUFFSIZE + 4];
			char *size = (char*)&length;
			char sizes[4] = {size[0], size[1], size[2], size[3]};
			printf("%s\n", sizes);
			// sprintf(paragraph, "%d", length);
			strcat(paragraph, sizes);
			strcat(paragraph, buff);
			if ((len2 = send(conn, paragraph, len + 4, 0)) < 0) {
				printf("Send Failed");
				fflush(stdout);
			}
			printf("Packets Sent: %d", len2);
			fflush(stdout);
			lastMessageSent = 1;
			// printf("%s", buff);
		}
	}

	// This check is to make sure at an EOF we still send the buffer if we were planning to
	if (!lastMessageSent) {
		uint32_t length = htonl(len);
		send(conn, buff, len, 0);
	}

	/* iteration ends when EOF found on stdin */

	(void) send_eof(conn);
	(void) printf("\n");
	return 0;
}
