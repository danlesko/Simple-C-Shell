/* Daniel Lesko
 * CMSC 421 Homework 2
 * Simple Shell Program
 *
 * Basic functionality with custom command overrides
 *
 */

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// for io and string manip
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// for execvp
#include <unistd.h>

// for wait()
#include <sys/wait.h>

// for sebald's functions
#include "utils.h"

// Present the user with a prompt at which he or she can enter commands.
void command_prompt();

// Accept command input of arbitrary length.
char * grab_line(int *);

// Parse command-line arguments from the user's input
char ** get_commands(char *);

// Pass commands along to the program that the user requests to be started.
void run_commands(char **);

// A built-in exit command that can take zero or one arguments that exits your shell.
// If an argument is specified, it must be an integer, which should be used as the exit code for the program.
// If a non-integer argument or multiple arguments is given, you must simply print out an error message and not exit.
// If no argument is given, assume that the exit code should be 0.
void custom_exit(char *, char **);

// A built-in chdir command that changes the current working directory of the shell.
// If no argument to this command is specified, then the directory shall be changed to the user's home directory
// (assuming HOME is set in the user's environment — if it is not set, then you shall print out an error). You must also be sure to also update the PWD environment variable.
void custom_cd(char *, char **);

// same as custom_cd
void custom_chdir(char *, char **);

// A built-in echo command that shall unescape the given string and print it out, followed by a newline on the user's standard output.
void custom_echo(char *, char **);

// A built-in setenv command that sets the specified environment variable name to a given value.
// The value shall be unescaped prior to storing it in the environment variable.
// The command shall be structured so that setenv key=value will set the environment variable key to the value.
// Anything after the full unescaped value may be ignored (so if you had setenv k="v 123" abc , the abc part would simply be ignored).
void custom_setenv(char *, char **);

// A built-in getenv command that looks up the specified environment variable (a string) and prints out the contents of it, followed by a newline character.
// If the specified environment variable does not exist in the user's environment, then you shall simply print out the newline character specified above.
// You should report an error if more than one argument is given to this command.
void custom_getenv(char *, char **);

// Helper function that parses the tokens correctly based on whether we are inside of double quotations or not
// http://stackoverflow.com/questions/9659697/parse-string-into-array-based-on-spaces-or-double-quotes-strings
char *tokenize_with_quotes(char *str, char **next);

// All built-in commands shall have priority over identically named programs that are in the user's PATH.
char *custom_commands[] = {
	"cd",
	"chdir",
	"echo",
	"exit",
	"setenv",
	"getenv"
};

// http://stackoverflow.com/questions/252748/how-can-i-use-an-array-of-function-pointers
void (*custom_functions[]) (char *, char **) = {
	&custom_cd,
	&custom_chdir,
	&custom_echo,
	&custom_exit,
	&custom_setenv,
	&custom_getenv
};


int main(/* not currently accepting command line args */) {

	// run the command line
	command_prompt();

	// currently program will never get to this. press ctrl + c to exit or type exit in console
	return 0;
}

char *tokenize_with_quotes(char *str, char **next)
{
	char *current = str;
	char *start = str;
	//int isQuoted = 0;

	// Eat beginning whitespace.
	while (*current && isspace(*current)) current++;
	start = current;

	if (*current == '"')
	{
		//isQuoted = 1;
		// Quoted token
		current++; // Skip the beginning quote.
		start = current;
		for (;;)
		{
			// Go till we find a quote or the end of string.
			while (*current && (*current != '"')) current++;
			if (!*current)
			{
				// Reached the end of the string.
				goto finalize;
			}
			if (*(current - 1) == '\\')
			{
				// Escaped quote keep going.
				current++;
				continue;
			}
			// Reached the ending quote.
			goto finalize;
		}
	}
	// Not quoted so run till we see a space.
	while (*current && !isspace(*current)) current++;

finalize:
	if (*current)
	{
		// Close token if not closed already.
		*current = 0;
		current++;
		// Eat trailing whitespace.
		while (*current && isspace(*current)) current++;
	}
	*next = current;

	//return isQuoted ? unescape(start, stderr) : start;
	return start;
}

void run_commands(char ** commandTokens) {

	// taken from class slides 3.24
	pid_t pid;

	pid = fork();

	if (pid < 0) { /* error occured */
		fprintf(stderr, "Fork Failed");
	}
	else if (pid == 0) { /* child process */
		// hint was given to use a function with a "p"
		// from the posix spec:
		// Using execvp()
		// The following example searches for the location of the ls command among the directories specified by the PATH environment variable, and passes arguments to the ls command in the cmd array.
		execvp(commandTokens[0], commandTokens);
	}
	else { /* parent process */
		/* parent will wait for the child to complete */
		wait(NULL);
		//printf("Child Complete\n");
	}
}

char * grab_line(int * size) {

	// beginning size of buffer allocation
	const int bufferChunk = 512;

	//point userInput to block of memory
	char * userInput = malloc(bufferChunk * sizeof(char));

	// http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
	if (!userInput) {
		printf("Failed to alloc buffer for user input!");
		return NULL;
	}

	int numRealloc = 1;
	int i = 0;
	char c;

	// http://stackoverflow.com/questions/4293475/reading-c-file-line-by-line-using-fgetc
	// grab each char in stream 1 at a time
	for (c = fgetc(stdin); c != '\n'; i++, c = fgetc(stdin)) {

		userInput[i] = c;

		// if the program has exceed the allocated buffer, reallocate memory
		if (i == bufferChunk) {
			numRealloc++;

			// error checking realloc as described in class
			void * tptr;
			if (!(tptr = realloc(userInput, numRealloc * bufferChunk * sizeof(char)))) {
				free(userInput);
				// other error handling
				printf("Failed to realloc buffer for user input!");
				return NULL;
			}
			userInput = tptr;
		}
	}



	userInput[i] = '\0';
	*size = i;
	return userInput;
}

char ** get_commands(char * userInput) {

	// beginning size of buffer allocation
	const int bufferChunk = 512;

	// point commandTokens to block of memory
	char ** commandTokens = malloc(bufferChunk * sizeof(char *));

	// http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
	if (!commandTokens) {
		printf("Failed to alloc buffer for command tokens!");
		return NULL;
	}

	// http://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
	// example for how to use strtok_r
	//char command[bufferChunk];
	//char *rest = userInput;

	int numRealloc = 1;
	int i = 0;
	//int startStr = 0;
	//int endStr;

	char * pText = malloc(strlen(userInput) * sizeof(char *));

	if (!pText) {
		printf("Failed to alloc buffer for user input!");
		return NULL;
	}

	strcpy(pText, userInput);

	//char *pText = userInput;


	while (*pText)
	{
		commandTokens[i] = tokenize_with_quotes(pText, &pText);

		if (i == bufferChunk) {
			numRealloc++;

			// error checking realloc as described in class
			void * newtptr;
			if (!(newtptr = realloc(commandTokens, numRealloc * bufferChunk * sizeof(char*)))) {
				free(commandTokens);
				// other error handling
				printf("Failed to realloc buffer for command tokens!");
				return NULL;
			}
			commandTokens = newtptr;
		}
		i++;
	}

	// free(pText);

	// for (;/*(command = strtok_r(rest, " ", &rest))*/;i++) {

	// 	endStr = first_unquoted_space(userInput);
	// 	memcpy(command, &userInput[startStr], endStr-1);
	// 	startStr = endStr;
	// 	command[endStr] = '\0';


	// 	commandTokens[i] = command;

//        // if the program has exceed the allocated buffer, reallocate memory
	// 	if (i == bufferChunk){
	// 		numRealloc++;

//        	// error checking realloc as described in class
	// 		void * newtptr;
	// 		if (!(newtptr = realloc(commandTokens, numRealloc * bufferChunk * sizeof(char*)))){
	// 			free(commandTokens);
	// 			// other error handling
	// 			printf("Failed to realloc buffer for command tokens!");
	// 			return NULL;
	// 		}
	// 		commandTokens = newtptr;
	// 	}
	// }

	commandTokens[i] = NULL;

	return commandTokens;
}



void custom_cd(char * userInput, char ** commandTokens) {
	// printf("Calling from cd command...\n");

	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2)
	{
		if (getenv("HOME") != NULL)
		{
			chdir(getenv("HOME"));
			setenv("PWD", getenv("HOME"), 1);
		}
		else
		{
			printf("Error invalid home directory");
		}
	}
	else
	{
		char * unescapedTok = unescape(commandTokens[1], stderr);
		chdir(unescapedTok);

		char * unescapedCWD = unescape(getcwd(NULL, 0), stderr);
		setenv("PWD", unescapedCWD, 1);
		free (unescapedTok);
		free (unescapedCWD);
	}
}

void custom_chdir(char * userInput, char ** commandTokens) {
	// printf("Calling from cd command...\n");

	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2)
	{
		if (getenv("HOME") != NULL)
		{
			chdir(getenv("HOME"));
			setenv("PWD", getenv("HOME"), 1);
		}
		else
		{
			printf("Error invalid home directory");
		}
	}
	else
	{
		char * unescapedTok = unescape(commandTokens[1], stderr);
		chdir(unescapedTok);

		char * unescapedCWD = unescape(getcwd(NULL, 0), stderr);
		setenv("PWD", unescapedCWD, 1);
		free (unescapedTok);
		free (unescapedCWD);
	}
}

void custom_echo(char * userInput, char ** commandTokens) {
	// printf("Calling from echo command...\n");

	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2)
	{
		// do nothing
		return;
	}
	else
	{
		int i = 0;
		for (i = 1; i < numTokens; i++) {
			printf("%s ", unescape(commandTokens[i], stderr));
		}
		printf("\n");
		//return args;
	}
}

void custom_exit(char * userInput, char ** commandTokens) {

	// printf("Calling from exit command function...\n");
	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2) {
		//printf("1...\n");
		free(userInput);
		free(commandTokens);
		exit(0);
	}
	else if (numTokens < 3) {
		//printf("2...\n");
		char *end;
		long num = strtol(commandTokens[1], &end, 10);
		int exitNum = (int) num;

		// http://stackoverflow.com/questions/19148611/using-strtol-to-validate-integer-input-in-ansi-c
		if (*end == '\0') {
			free(userInput);
			free(commandTokens);
			exit(exitNum);
		}

	}

	printf("Ya done goofed...\n");
}

void custom_setenv(char * userInput, char ** commandTokens) {
	// printf("Calling from setenv command...\n");

	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2) {
		printf("No arguments\n");
		return;
	} else {

		// printf("BP 1\n");
		//char * env = strtok(commandTokens[1], "=");
		char * tempCommand = strchr(commandTokens[1], '=');
		//char * env = strchr(commandTokens[1], '=');
		//char * env = *tempCommand - commandTokens[1];

		size_t len = tempCommand - commandTokens[1];
		char * env = malloc(len + 1);

		if (!env) {
			printf("Failed to alloc buffer for user input!");
			return;
		}

		memcpy(env, commandTokens[1], len);
		env[len] = 0;

		//tempCommand = tempCommand + 1;

		//commandTokens[1] = tempCommand;

		// printf("BP 2\n");

		char * inputPtr = strchr(userInput, '=');
		inputPtr = inputPtr + 1;

		char * tempInput = malloc(strlen(userInput) * sizeof(char));

		if (!tempInput) {
			printf("Failed to alloc buffer for user input!");
			return;
		}

		strcpy(tempInput, inputPtr);

		// printf("BP 3\n");

		/*char ** setenvTokens = get_commands(tempInput);

		int numEnvTokens = 0;

		while(setenvTokens[numEnvTokens] != NULL){
			numEnvTokens++;
		}

		for (int i = 0; i < numEnvTokens; i++){
			printf("%s\n", unescape(setenvTokens[i], stderr));
			//printf("%s\n", setenvTokens[i]);
		}*/

		//printf("%s\n", env/* unescape(commandTokens[1], stderr)*/);
		// printf("%s\n", tempCommand/* unescape(commandTokens[1], stderr)*/);
		//printf("%s\n", tempInput);

		char * unescapedTempInput = unescape(tempInput, stderr);

		setenv(env, unescapedTempInput, 1);

		// printf("BP 4\n");

		free(env);
		free(tempInput);
		free(unescapedTempInput);

		// printf("BP 5\n");
	}
}

void custom_getenv(char * userInput, char ** commandTokens) {
	// printf("Calling from getenv command...\n");

	int numTokens = 0;

	while (commandTokens[numTokens] != NULL) {
		numTokens++;
	}

	if (numTokens < 2) {
		printf("No arguments\n");
		return;
	}
	else if (numTokens < 3)
	{
		if (commandTokens[1] != NULL)
		{
			if (getenv(commandTokens[1]) != NULL)
			{
				printf("%s\n", getenv(commandTokens[1]));
				//printf("what?\n");
				return;
			}
			else
			{
				printf("\n");
				return;
			}
		}
		else
		{
			return;
		}

	}
	else
	{
		printf("Error invalid number of arguments\n");
		return;
	}
}


void command_prompt() {
	int inputSize;
	char * userInput;
	char ** commandTokens;
	//char * exitToken = "";

	// probably bad practice to set this here but oh well
	int customCommandsNum = 6;

	// main loop for the shell program!
	// in class suggestion to use a while loop
	// run loop until user types exit
	for (;;) {
		//int i = 0;
		int runReg = 1;

		// Present the user with a prompt at which he or she can enter commands.
		printf(ANSI_COLOR_GREEN "$: " ANSI_COLOR_RESET);

		// Accept command input of arbitrary length
		userInput = grab_line(&inputSize);
		// printf("The inputsize is:%d\n", inputSize);

		// user just presses enter
		if (userInput == NULL || userInput[0] == '\0') {
			//printf("Break here?\n");
			continue;
		}


		// Parse command-line arguments from the user's input
		commandTokens = get_commands(userInput);



		// tests to see if the command is in our local shell commands
		int i = 0;
		for (i = 0; i < customCommandsNum; i++) {
			if (strcmp(commandTokens[0], custom_commands[i])  == 0) { //expression equates to 0 when true
				//printf("This work?\n");
				(*custom_functions[i])(userInput, commandTokens);
				runReg = 0;

				break;
			}
		}

		if (runReg) {
			run_commands(commandTokens);
		}

		// Pass them along to the program that the user requests to be started.
		//    	if (strcmp(commandTokens[0], "exit")){ //expression equates to 0 when true
		// 	run_commands(commandTokens);
		// }
		// else{
		// 	free(userInput);
		// 	free(commandTokens);
		// 	exit(1);
		// }

		free(userInput);
		free(commandTokens);
	}



}