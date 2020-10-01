#Shell Command#
###How to Download### 
1. Download the zip-file and extract it
2. Open your terminal and change the directory to the extracted location ( extractedLocation/ShellProject/ )
3. Compile the main file with gcc main.c (if you want specified name do gcc -o _name_ main.c)
4. Run the compiled version with ./a.out (or if you have specified a name do ./name)


### Implemented features ###
##### This shell will take the following inputs as arguments #####
###ls   
Ls will output the content of a directory to the terminal. The arguments that is implemented in this program is: ls (-a) (/_DirName_) (| grep _grepWord_)   
Everything inside the parenthesis is optional and can be combined together.

###cd   
Cd will change your directory into a specified one if it exists.   
The arguments implemented in this program is: cd (~) (_DirName_) (_/fullPath_)   
Everything inside the parenthesis is optional and if its only cd then you will go to the home directory.   
None of these can be combined.

### clear
Clear will clear the terminal window.

### pwd
Pwd will output the current working directory to the terminal.

### mkdir
Mkdir will create a new directory in the current working directory if it doesnt already exist.  
Mkdir will take a single argument _dirName_ which will be the name of the new directory.
 
### rmdir
Rmdir will delete an existing directory in the current working directory.  
Rmdir will take a single argument _dirName_ which will be the name of the directory that needs to be removed.

### grep
Grep will output the content of a file to the terminal where grepWord is present.   
The implemented arguments for grep is: grep _grepWord_ _fileName_

###cat
Cat will output the content of a file to the terminal.   
The arguments implemented in this program is: cat _fileName_ ( | grep _grepWord_ )
Everything inside the parenthesis is optional.   

###exit 
Exit will close down the program
