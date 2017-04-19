# shell-landesko

## HW2Final

The requirements for the second simple shell extension program required a lot of research, and remembering how to do various things in C. This simple shell took the existing code for the first shell and added to it to allow for custom commands that we define inside the code. Professor Sebald provided some useful utility functions that are included in utils.h and utils.c which made escaping some of the user input much easier. A list of the exact requirements for this shell can be found here:

https://bluegrit.cs.umbc.edu/~lsebald1/cmsc421-sp2017/hw2.shtml

First and foremost we needed our user commands to take precedence over commands found in /bin/. In order to implement this, I created an array of function pointers as well as an array of custom command names. When an user entered a command, I would check to see if the command matched one of our custom commands. If it did, the correct function would be called and a flag would be set to let us know that a custom command had been called and that we did not need to try and run the command and command tokens with execvp().

Discussions in class and Piazza also helped point me in the right direction for tokenizing the userInput as we now wished to tokenize user input based on whether it was enquoted in double quotes or, if not equoted, separated by spaces. We had to do this to ensure our tokens would work correctly with the custom functions we had to implement. After checking on Piazza, I did some google searching and located a stackoverflow question where an user was trying to do the same thing. One of the answers involved creating a state machine which checked whether or not we were inside double quotes or not when tokenizing user input, and tokenizing the user input accordingly. Using that code as a start, I made some small modifications that tokenized the user input string perfectly. 

For all of the custom commands, implementation was rather easy once the tokenization was figured out. The hard part was figuring out when and where to free various user input and strings returned by Sebald's unescape() function. However, generally each command involved counting the number of command tokens available, making sure they weren't NULL, and calling the appropriate function of the same name from a C library. This took a little bit of research, but ultimately was fairly straightforward. This is true of cd/chdir/getenv/setenv/exit. The only function that didn't call a function of the C library was echo, where we simply printed out the command tokens in order, separated by a space, after they were unescaped of course. 

### References:

- Class lectures
- Class slides
- http://stackoverflow.com/questions/5607455/checking-that-malloc-succeeded-in-c
- http://stackoverflow.com/questions/4293475/reading-c-file-line-by-line-using-fgetc
- http://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html
- http://pubs.opengroup.org/onlinepubs/9699919799/functions/fexecve.html

### Extension References:

- https://linux.die.net/man/3/getcwd
- http://man7.org/linux/man-pages/man3/getenv.3.html
- http://man7.org/linux/man-pages/man3/setenv.3.html
- http://stackoverflow.com/questions/19148611/using-strtol-to-validate-integer-input-in-ansi-c
- http://stackoverflow.com/questions/9659697/parse-string-into-array-based-on-spaces-or-double-quotes-strings
- http://stackoverflow.com/questions/252748/how-can-i-use-an-array-of-function-pointers
