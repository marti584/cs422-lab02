/* echoserver.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cnaiapi.h>

#define BUFFSIZE		1000000
#define INITIAL_OUTPUT	"Received Paragraph >\n"

/*-----------------------------------------------------------------------
 *
 * Program: echoserver
 * Purpose: wait for a connection from an echoclient and echo data
 * Usage:   echoserver <appnum>
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	connection	conn;
	int			len;
	char		buff[BUFFSIZE];

	if (argc != 2) {
		(void) fprintf(stderr, "usage: %s <SERVER_APPLICATION_NUMBER>\n", argv[0]);
		exit(1);
	}

	/* wait for a connection from an echo client */

	conn = await_contact((appnum) atoi(argv[1]));
	if (conn < 0)
		exit(1);

	/* iterate, echoing all data received until end of file */

	while((len = recv(conn, buff, BUFFSIZE, 0)) > 0) {
		printf("%s", INITIAL_OUTPUT);
		uint32_t len = ntohl(buff);
		printf("%d", len);
	}

	send_eof(conn);
	return 0;
}
