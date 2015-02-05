#include <stdio.h>
#include <stdlib.h>
#include <hardware.h>
#include <terminals.h>

#define MAX_ECHO_BUFFER_LENGTH 1024
/*
 * The state of each of the terminals
 * 
 * This is declared 'static' so it can't be seen outside this .c file.
 */
static int state[MAX_NUM_TERMINALS];
#define	READING		0
#define	WRITING		1
#define	SITTING		2

static int echoBufferLength[MAX_NUM_TERMINALS];
static int echoItemsLeft[MAX_NUM_TERMINALS];
static char echoBuffer[MAX_NUM_TERMINALS][MAX_ECHO_BUFFER_LENGTH];
static int isEchoing = 0;
/*
 * Output buflen chars from buf to screen, blocking until all characters
 * are displayed.  Returns the number of characters written or -1 for 
 * error. Writes to terminal term.
 */
int WriteTerminal(int term, char *buf, int buflen) {

	return -1;
}

/*
 * Read characters from terminal and place into buf until buflen chars
 * have been read or a '\n' is read.  Returns the number of characters
 * read or -1 for error.  Like the Unix read() call but unlike normal
 * C programming language strings, no null character is automatically
 * added onto the end of the buffer by this call.  Reads from
 * terminal term.
 */
int ReadTerminal(int term, char *buf, int buflen) {
	return -1;
}

/*
 * Must be called once by application before any invocation of 
 * WriteTerminal or ReadTerminal on terminal number term.  Must call
 * the hardware's InitHardware for term.  Returns 0 if OK, or -1
 * for any error.
 */
int InitTerminal(int term) {	
	echoBufferLength[term] = 0;
	echoItemsLeft[term] = 0;
	return 0;
}

/*
 * Must be called exactly once before any other terminal API calls.
 * Initializes the terminal driver itself, not any particular
 * terminal.  Returns 0 if OK, or -1 for any error.
 */
int InitTerminalDriver(void) {
	return -1;
}

/*
 * ----------------------------------------------------------------------
 * IMPORTANT:
 * You *must* write routines as part of your terminal driver to handle
 * these two interrupts.  The procedure names must be "TransmitInterrupt"
 * and "ReceiveInterrupt" as shown below:
 * ----------------------------------------------------------------------
 */

/*
 * TransmitInterrupt is called once for each character written to the
 * data register after the character has been written to the screen
 * of the terminal.
 */
void TransmitInterrupt(int term) {
	printf("TransmitInturrupt\n");
	if (isEchoing) {
		isEchoing = 0;		
		echoItemsLeft[term]--;
	}
	if (echoItemsLeft[term] != 0) {
		isEchoing = 1;		
		char reg_char=echoBuffer[term][echoBufferLength[term] % MAX_ECHO_BUFFER_LENGTH];
		WriteDataRegister(term, reg_char);
	}
}

/*
 * ReceiveInterrupt is called once for every character typed on the
 * keyboard of the terminal after the character has been placed in the
 * data register 
 */
void ReceiveInterrupt(int term) {
	printf("RecieveInturrupt %d\n", term);
	char reg_char = ReadDataRegister(term);
	printf("Char is %c\n", reg_char);
	echoBuffer[term][echoBufferLength[term]];
	echoItemsLeft[term]++;
	echoBufferLength[term]++;
	if (!isEchoing) {
		isEchoing = 1;
		WriteDataRegister(term, reg_char);
	}
}
