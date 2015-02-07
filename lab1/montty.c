#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hardware.h>
#include <terminals.h>
#include <threads.h>

#define MAX_ECHO_BUFFER_LENGTH 1024
#define MAX_WRITE_BUFFER_LENGTH 1024

/*
 * The state of each of the terminals
 * 
 * This is declared 'static' so it can't be seen outside this .c file.
 */
static int state[MAX_NUM_TERMINALS];
#define	SITTING		0
#define	READING		1
#define	WRITING		2
#define ECHOING     3
static int echoBufferLength[MAX_NUM_TERMINALS];
static int echoItemsLeft[MAX_NUM_TERMINALS];
static char echoBuffer[MAX_NUM_TERMINALS][MAX_ECHO_BUFFER_LENGTH];
static int echoBufferSpot[MAX_NUM_TERMINALS];

static int writeItemsLeft[MAX_NUM_TERMINALS];
static char writeBuffer[MAX_NUM_TERMINALS][MAX_WRITE_BUFFER_LENGTH];
static int writeBufferLength[MAX_NUM_TERMINALS];

/*
 * Output buflen chars from buf to screen, blocking until all characters
 * are displayed.  Returns the number of characters written or -1 for 
 * error. Writes to terminal term.
 */
extern int WriteTerminal(int term, char *buf, int buflen) {
	Declare_Monitor_Entry_Procedure();
	printf("Writing terminal %d\n", term);

	while (state[term] == WRITING) {
		CondWait(term);
	}
	state[term] = WRITING;

	memcpy(writeBuffer[term], buf, buflen);
	writeItemsLeft[term] = buflen;
	char reg_char = writeBuffer[term][0];
	writeBufferLength[term] = 0;
		printf("3\n");
	writeItemsLeft[term]--;
	WriteDataRegister(term, reg_char);
		printf("c\n");
//	CondSignal(term);
	return 0;
}

/*
 * Read characters from terminal and place into buf until buflen chars
 * have been read or a '\n' is read.  Returns the number of characters
 * read or -1 for error.  Like the Unix read() call but unlike normal
 * C programming language strings, no null character is automatically
 * added onto the end of the buffer by this call.  Reads from
 * terminal term.
 */
extern int ReadTerminal(int term, char *buf, int buflen) {
	Declare_Monitor_Entry_Procedure();
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
	writeItemsLeft[term] = 0;
	echoBufferSpot[term] = 0;
    InitHardware(term);

	return 0;
}

/*
 * Must be called exactly once before any other terminal API calls.
 * Initializes the terminal driver itself, not any particular
 * terminal.  Returns 0 if OK, or -1 for any error.
 */
int InitTerminalDriver(void) {
	return 0;
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
	// Declare_Monitor_Entry_Procedure();
	printf("TransmitInturrupt\n");
	char reg_char = 0;
	state[term] = SITTING;

	if (echoItemsLeft[term] != 0) {
		printf("echoagain %d\n", echoItemsLeft[term]);
		echoItemsLeft[term]--;
		reg_char = echoBuffer[term][echoBufferSpot[term] % MAX_ECHO_BUFFER_LENGTH];
		echoBufferSpot[term]++;
		printf("regchar is %c\n", reg_char);
	}
	else {
		if (writeItemsLeft[term] != 0) {
			writeItemsLeft[term]--;
			writeBufferLength[term]++;
			reg_char = writeBuffer[term][writeBufferLength[term]];
		}
	}

	if (reg_char != 0) {
		state[term] = WRITING;
		WriteDataRegister(term, reg_char);
	}
		

}

/*
 * ReceiveInterrupt is called once for every character typed on the
 * keyboard of the terminal after the character has been placed in the
 * data register 
 */
void ReceiveInterrupt(int term) {
	// Declare_Monitor_Entry_Procedure();
	printf("RecieveInturrupt %d\n", term);
	printf("term %d\n", term);
	char reg_char = ReadDataRegister(term);
	printf("Char is %c\n", reg_char);
	echoBuffer[term][echoBufferLength[term] % MAX_ECHO_BUFFER_LENGTH] = reg_char;
	echoItemsLeft[term]++;
	echoBufferLength[term]++;
	printf("State %d\n", state[term]);
	if (state[term] == SITTING) {
		state[term] = ECHOING;
		echoItemsLeft[term]--;
		echoBufferSpot[term]++;

		printf("items %d\n", echoItemsLeft[term]);
		WriteDataRegister(term, reg_char);
	}
}
