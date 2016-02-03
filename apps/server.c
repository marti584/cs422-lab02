/* echoserver.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cnaiapi.h>
#include <netinet/in.h>

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
	char 		size[4];
	char		buff[BUFFSIZE];

	if (argc != 2) {
		(void) fprintf(stderr, "usage: %s <SERVER_APPLICATION_NUMBER>\n", argv[0]);
		exit(1);
	}

	/* wait for a connection from an echo client */

	conn = await_contact((appnum) atoi(argv[1]));
	if (conn < 0)
		exit(1);

	// Waits for 4 bytes to come through because we know that will
	// be the size of the paragraph
	while((len = recv(conn, size, 4, 0)) > 0) {

		// Print that we've received a paragraph
		printf("%s", INITIAL_OUTPUT);

		// These 4 bytes are the size of the paragraph
		// So we are converting it from the Network Endianness
		// to our host Endianness
		uint32_t len = ntohl(*((unsigned long*) size));

		// Keep track of the number bytes read from the paragraph
		int bytesRead = 0;

		// Keep concatenating on this char array for the whole paragraph
		char paragraph[BUFFSIZE];

		// Keep reading until we have all the bytes
		while (bytesRead < len) {

			// Smaller buffer for the characters read on this recv() call
			char charactersRead[BUFFSIZE];
			int lengthOfBytes;

			// Read some bytes from the sender
			if (lengthOfBytes = recv(conn, charactersRead, BUFFSIZE, 0) > 0) {
				// Concatenate our paragraph with the bytes that were read
				strcat(paragraph, charactersRead);

				// Increment the counter with the size of the bytes read
				bytesRead += strlen(charactersRead);

				// Clear the charactersRead buffer just for security
				memset(&charactersRead[0], 0, sizeof(charactersRead));
			}
		}

		// Print the paragraph
		printf("%s", paragraph);

		fflush(stdout);

		// Clear the paragraph buffer
		memset(&paragraph[0], 0, sizeof(paragraph));
	}

	send_eof(conn);
	fflush(stdout);
	return 0;
}
