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
#include <stdlib.h>
#include <string.h>

// for execvp
#include <unistd.h>
 
// for wait()
#include <sys/wait.h>

// Present the user with a prompt at which he or she can enter commands.
void command_prompt();

// Accept command input of arbitrary length.
char * grab_line();

// Parse command-line arguments from the user's input
char ** get_commands(char * userInput);

// Pass commands along to the program that the user requests to be started. 
void run_commands(char **);


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

void command_prompt(){
	char * userInput;
	char ** commandTokens;
	char * exitToken = "";

	// main loop for the shell program!
	// in class suggestion to use a while loop
	// run loop until user types exit
  	for(;;){

  		// Present the user with a prompt at which he or she can enter commands.
  		printf("$: ");

  		// Accept command input of arbitrary length
  		userInput = grab_line();

	    // Parse command-line arguments from the user's input
	    commandTokens = get_commands(userInput);

	    // Pass them along to the program that the user requests to be started.
	    if (strcmp(commandTokens[0], "exit")){
			run_commands(commandTokens);
		}
		else{
			free(userInput);
			free(commandTokens);
			exit(1);
		}

		free(userInput);
		free(commandTokens);
  	}

  	

}