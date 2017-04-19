#shell-landesko
##HW1Final

The requirements for the first simple shell program required a lot of research, and remembering how to do various things in C. Luckily, a few helpful functions were linked by Professor Sebald, and a lot of how to start the project and do error handling was discussed in class. I knew from the start that I would have to break up each chunk of the program to correspond with the three requirements:

1) Present the user with a prompt at which he or she can enter commands.  
2) Accept command input of arbitrary length.  
3) Parse command-line arguments from the user's input and pass them along to the program that the user requests to be started.  

The last requirement asked us to do two things so it could be further broken up as such:

3a) Parse command-line arguments from the user's input  
3b) Pass them along to the program that the user requests to be started. 

For Requirement #1, all we had to do was present the user with a command prompt. I made this a function of its own. I decided to use the for(;;) loop, as I had never used a for loop like that before. Inside the loop I then prompt the user with with the command prompt "$:". It was also here that I initialized the variables I would use for reading in the userString and tokenizing the input.

Requirement #2 required us to accept a string from an user. It was mentioned many times in class that we would have to handle a string of any possible length. To read in the string, I used fgetc and passed in stdin as the argument. This allowed me to read character by character, what the user had entered. I knew that I had to accept any size string. This requires us to allocate a chunk of memory, and when it is full, reallocate. I reallocated by using a counter and comparing it to the multiplier that I had already allocated, called bufferChunk. When my counter reached the same value as bufferChunk, I reallocated. The reallocation code that I used came straight from class, it was written on the board by the professor. This method allowed me to successfully read in a new array, which I could then return back to the command prompt. 

Requirement #3a required us to tokenize the user input string into commands. I found that using the strtok_r would probably work best for me here. I implemented this function in much the same way what the first function is implemented, in terms of reallocating memory. I did this because it allowed any number of commands to possibly be passed, even though there generally aren't that many. Once I was done tokenizing the user's input, and storing the commands, I returned the array of commands.

Requirement #3b was probably the easiest. The code used to basically run the fork function was listed in the slides. The only modification I made was using the execvp function, which was hinted to use about in class. (That we would need to use a function that had a 'p' in it). 


#References:

- Class lectures
- Class slides
- http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
- http://stackoverflow.com/questions/4293475/reading-c-file-line-by-line-using-fgetc
- http://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/fexecve.html
