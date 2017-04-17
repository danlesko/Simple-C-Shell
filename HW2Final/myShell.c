/* Daniel Lesko
 * CMSC 421 Homework 1
 * Simple Shell Program
 *
 * Functionality: 
 * Present the user with a prompt at which he or she can enter commands.
 * Accept command input of arbitrary length.
 * Parse command-line arguments from the user's input
 * Pass them along to the program that the user requests to be started. 
 */

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
char * grab_line();

// Parse command-line arguments from the user's input
char ** get_commands(char *);

// Pass commands along to the program that the user requests to be started. 
void run_commands(char **);

void custom_exit(char *, char **);

void custom_cd(char *, char **);

void custom_chdir(char *, char **);

void custom_echo(char *, char **);

void custom_setenv(char *, char **);

void custom_getenv(char *, char **);

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


int main(/* not currently accepting command line args */){

	// run the command line
	command_prompt();

	// currently program will never get to this. press ctrl + c to exit
  	return 0;
}

void run_commands(char ** commandTokens){

	// taken from class slides 3.24
	pid_t pid;

    pid = fork();

    if (pid < 0){ /* error occured */
    	fprintf(stderr, "Fork Failed");
	}
	else if (pid == 0) { /* child process */
		// hint was given to use a function with a "p"
		// from the posix spec:
		// Using execvp()
		// The following example searches for the location of the ls command among the directories specified by the PATH environment variable, and passes arguments to the ls command in the cmd array.
		execvp(commandTokens[0], commandTokens);
	}
	else{ /* parent process */
		/* parent will wait for the child to complete */
		wait(NULL);
		//printf("Child Complete\n");
	}
}

char * grab_line(){

	// beginning size of buffer allocation
	const int bufferChunk = 512;

	//point userInput to block of memory
  	char * userInput = malloc(bufferChunk * sizeof(char));

  	// http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
  	if (!userInput){
  		printf("Failed to alloc buffer for user input!");
  		return NULL;
  	}

  	int numRealloc = 1;
  	int i = 0;
  	char c;

  	// http://stackoverflow.com/questions/4293475/reading-c-file-line-by-line-using-fgetc
  	// grab each char in stream 1 at a time
  	for (c = fgetc(stdin);c != '\n';i++, c = fgetc(stdin)){
	    
	    userInput[i] = c;

	    // if the program has exceed the allocated buffer, reallocate memory
	    if (i == bufferChunk){
	    	numRealloc++;

        	// error checking realloc as described in class
        	void * tptr;
			if (!(tptr = realloc(userInput, numRealloc * bufferChunk * sizeof(char)))){
				free(userInput);
				// other error handling
				printf("Failed to realloc buffer for user input!");
				return NULL;
			}
			userInput = tptr;
        }
	}

	userInput[i] = '\0';
	return userInput;
}

char ** get_commands(char * userInput){

	// beginning size of buffer allocation
	const int bufferChunk = 512;

	// point commandTokens to block of memory
    char ** commandTokens = malloc(bufferChunk * sizeof(char *));

    // http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
    if (!userInput){
  		printf("Failed to alloc buffer for command tokens!");
  		return NULL;
  	}

    // http://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
    // example for how to use strtok_r
    char *command;
    char *rest = userInput;

 	int numRealloc = 1;
 	int i = 0;
  	for (;(command = strtok_r(rest, " ", &rest));i++) {

        commandTokens[i] = command;

        // if the program has exceed the allocated buffer, reallocate memory
	    if (i == bufferChunk){
        	numRealloc++;

        	// error checking realloc as described in class
        	void * newtptr;
			if (!(newtptr = realloc(commandTokens, numRealloc * bufferChunk * sizeof(char*)))){
				free(commandTokens);
				// other error handling
				printf("Failed to realloc buffer for command tokens!");
				return NULL;
			}
			commandTokens = newtptr;
        }
    }

    commandTokens[i] = NULL;

    return commandTokens;
}

void custom_cd(char * userInput, char ** commandTokens){


}

void custom_chdir(char * userInput, char ** commandTokens){
	printf("Calling from exit command...");
}

void custom_echo(char * userInput, char ** commandTokens){
	printf("Calling from exit command...");
}

// Command that can take zero or one arguments that exits your shell. 
// If an argument is specified, it must be an integer, which should be used as the exit code for the program. 
// If a non-integer argument or multiple arguments is given, you must simply print out an error message and not exit. 
// If no argument is given, assume that the exit code should be 0.
void custom_exit(char * userInput, char ** commandTokens){

	printf("Calling from exit command function...\n");
	int numTokens = 0;

	while(commandTokens[numTokens] != NULL){
		numTokens++;
	}

	if (numTokens < 2){
		printf("1...\n");
		free(userInput);
		free(commandTokens);
		exit(0);
	}
	else if (numTokens < 3){
		printf("2...\n");
		char *end;
		long num = strtol(commandTokens[1], &end, 10);
		int exitNum = (int) num;

		// http://stackoverflow.com/questions/19148611/using-strtol-to-validate-integer-input-in-ansi-c
		if (*end == '\0'){
			free(userInput);
			free(commandTokens);
			exit(exitNum);
		}
		
	} 

	printf("Ya done goofed...\n");
}

void custom_setenv(char * userInput, char ** commandTokens){
	printf("Calling from exit command...");
}

void custom_getenv(char * userInput, char ** commandTokens){
	printf("Calling from exit command...");
}


void command_prompt(){
	char * userInput;
	char ** commandTokens;
	char * exitToken = "";

	int customCommandsNum = 6;

	// main loop for the shell program!
	// in class suggestion to use a while loop
	// run loop until user types exit
  	for(;;){
  		int i = 0;
  		int runReg = 1;

  		// Present the user with a prompt at which he or she can enter commands.
  		printf("$: ");

  		// Accept command input of arbitrary length
  		userInput = grab_line();

	    // Parse command-line arguments from the user's input
	    commandTokens = get_commands(userInput);

	    for(i = 0; i < customCommandsNum; i++){
	    	if (strcmp(commandTokens[0], custom_commands[i])  == 0){ //expression equates to 0 when true
				//printf("This work?\n");
				(*custom_functions[i])(userInput, commandTokens);
				runReg = 0;

				break;
			} 
	    }

	    if(runReg)
	    	run_commands(commandTokens);



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