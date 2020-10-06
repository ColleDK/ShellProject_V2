Shell Command
This program is the 2nd iteration of my shell which will use system calls instead of hardcoded functions from https://github.com/ColleDK/ShellProject

How to Download
1. Download the zip-file and extract it
2. Open your terminal and change the directory to the extracted location ( extractedLocation/ShellProject/ )
3. Compile the main file with gcc shellProject.c (if you want specified name do gcc -o name shellProject.c)
4. Run the compiled version with ./a.out (or if you have specified a name do ./name)

Implemented features
This shell will take the following inputs as arguments

ls
Ls will output the content of a directory to the terminal. The arguments that is implemented in this program is: ls (-letter) (/DirName) (| grep grepWord)
Everything inside the parenthesis is optional and can be combined together.

cd
Cd will change your directory into a specified one if it exists.   
The arguments implemented in this program is: cd (~) (DirName) (/fullPath)
Everything inside the parenthesis is optional and if its only cd then you will go to the home directory.   
None of these can be combined.

clear
Clear will clear the terminal window.

pwd
Pwd will output the current working directory to the terminal.

mkdir
Mkdir will create a new directory in the current working directory if it doesnt already exist.
Mkdir will take a single argument dirName which will be the name of the new directory.

rmdir
Rmdir will delete an existing directory in the current working directory.
Rmdir will take a single argument dirName which will be the name of the directory that needs to be removed.

grep
Grep will output the content of a file to the terminal where grepWord is present.
The implemented arguments for grep is: grep grepWord fileName

cat
Cat will output the content of a file to the terminal.
The arguments implemented in this program is: cat fileName ( | grep grepWord ) ( > fileName)
Everything inside the parenthesis is optional, but grep and I/O redirection does not work together here.   

exit
Exit will close down the program

How it's implemented
This program uses a UNIX system's system calls to run these commands.
Whenever one of these commands needs to be run I use fork to create a child-process which will create an identical image of the parent-process, that will then execute a command e.g. ls or cat.   
When piping is utilized in the commands the user will run, I need to create an array that will be my read/write pipe. Since the output from the first child-process' execution doesn't need to be output to the user, I use the dup2 call to redirect my I/O to the read/write of the pipe.    
Whenever I run an execution i might need to run some code after the child-process has finished its execution, but **only** after it has finished. For example in my pipe i cannot run the grep command before i have run the cat command, therefore i use the system call wait, which will make the program idle for a certain amount of time (i use wait(NULL) to specify i need to wait for the child-process to finish).

Bibliography
1. https://codeforwin.org/2018/03/c-program-check-file-or-directory-exists-not.html
2. https://stackoverflow.com/questions/13450809/how-to-search-a-string-in-a-char-array-in-c
3. http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
4. https://www.quora.com/How-do-I-read-and-display-a-full-sentence-in-C