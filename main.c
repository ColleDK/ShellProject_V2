#include <fcntl.h>
#include "main.h"

/**
 * Global variables
 */
int TRUE = 1; // endless loop
int isPipe = 0; // check for pipe
int isIODir = 0; // check for IO direction
char slash[1]; // to make temporary paths
char pwd[100]; // path to current directory


/**
 * Function to check whether a directory exists or not.
 * It returns 1 if given path is directory exists
 * otherwise returns 0.
 * https://codeforwin.org/2018/03/c-program-check-file-or-directory-exists-not.html
 */
int isDirectoryExists(const char *path)
{
    struct stat stats;
    stat(path, &stats);
    if (S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}


/**
 * @param arr is a pointer to an array that needs to be erased of content
 * @param length is the length of content in the array (not length of array)
 */
void arrayCleaner(char* arr, int length){
    for (int i = 0; i < length; ++i) {
        arr[i] = '\0';
    }
}

/**
 * @param arr is a pointer to an array that needs to be split
 * @param inputNumber is the numbered input you want
 * @return returns a pointer to a new array
 */

char* inputSplitter(char* arr, int inputNumber){
    int spaceCounter=0, placeCounter=0, startpoint=0, endpoint=0;
    // i need a i-th number input therefore i move i-1 number of spaces up the array
    while (inputNumber-1 != spaceCounter){
        while (arr[placeCounter] != ' ' && arr[placeCounter] != '\0' && arr[placeCounter] != '\n'){
            placeCounter++;
        }
        spaceCounter++;
        // if multiple spaces in a row -> skip it
        while (arr[++placeCounter] == ' '){
        }
    }

    // this is input we want
    startpoint = placeCounter;
    while (arr[placeCounter] != ' ' && arr[placeCounter] != '\0' && arr[placeCounter] != '\n'){
        placeCounter++;
    }
    endpoint = placeCounter;

    // make space on heap for the input and return the pointer
    char *array = malloc((endpoint-startpoint) * sizeof(char));
    arrayCleaner(array, strlen(array)/sizeof(char));
    int loopCounter=0;
    for (int i = startpoint; i < endpoint; ++i) {
        array[loopCounter++] = arr[i];
    }
    return array;
}

/**
 * Change directory
 */
void cdCommand(char* secondInput) {
    if (strcmp("~",secondInput) == 0 || strcmp("",secondInput) == 0){ //checks if the secondinput is empty or ~ then it should go to home directory
        chdir(getenv("HOME"));
        getcwd(pwd,100); // update internal pwd
    }
    else {
        chdir(pwd);
        chdir(secondInput);
        getcwd(pwd, 100);
    }
}

/**
 * https://stackoverflow.com/questions/845556/how-to-ignore-hidden-files-with-opendir-and-readdir-in-c-library
 * Searches the directory for all files except for ones that start with . or .. (hidden files)
 */
void lsCommand(char* secondInput, char* thirdInput, char* fourthInput, char* fifthInput, char* sixthInput) {
    // check if there is pipe
    if (isPipe == 0) {
        int pid = fork(); // make a child process
        if (pid < 0){
            perror("ERROR ");
            exit(0);
        }
        else if (pid > 0) {
            wait(NULL); // wait for child process to finish
        } else {
            char *myargs[4]; // max amount is 3 inputs + NULL end
            myargs[0] = strdup("ls"); // duplicate ls into arguments
            if (strcmp("", secondInput) != 0) { //check if there is another input if yes insert it else put NULL
                myargs[1] = strdup(secondInput);
            } else {
                myargs[1] = NULL;
            }
            if (strcmp("", thirdInput) != 0) { // same as above
                myargs[2] = strdup(thirdInput);
            } else { myargs[2] = NULL; }
            myargs[3] = NULL; // last argument is always NULL

            execvp(myargs[0], myargs); // execute ls command
        }
    }


    else {
        // if pipe is enabled (only grep is allowed in program) check what space the grep is at and execute command
        if (strcmp(secondInput, "|") == 0) {
            if (strcmp("grep", thirdInput) == 0) {
                int pipefd[2]; // create an array which is used as our pipe
                int pid;

                pipe (pipefd); // set up the pipe
                pid=fork(); // make child process

                if(pid<0){ // fork error
                    perror("ERROR");
                    exit(0);
                }
                else if (pid==0)
                {
                    //in child proc
                    close (pipefd[0]); // child doesnt read
                    dup2(pipefd[1],1); // set output to the pipe write
                    char* myargs[2] = {"ls", NULL};
                    execvp(myargs[0],myargs); // execute ls command

                }else {
                    // in parent proc
                    pid = fork(); // fork again so we dont use parent to execute command
                    if (pid == 0) {
                        close(pipefd[1]); // parent doesn't write
                        dup2(pipefd[0], 0); //set input as the output from child
                        char *myargs[3] = {"grep", fourthInput, NULL}; // use grep command with specified word
                        execvp(myargs[0], myargs); //execute grep command
                    }
                    else{
                        wait(NULL); //wait for grep child
                    }
                    close(pipefd[0]); // close the pipe read
                    close(pipefd[1]); // close the pipe write
                    wait(NULL); // wait for ls child
                }
            }
        }

        // this is for ls -a | grep grepWord
        if (strcmp(thirdInput, "|") == 0) {
            if (strcmp("grep", fourthInput) == 0) {
                int pipefd[2]; // create an array which is used as our pipe
                int pid;

                pipe (pipefd); // set up the pipe
                pid=fork(); // make child process

                if(pid<0){ // fork error
                    perror("ERROR");
                    exit(0);
                }
                else if (pid==0)
                {
                    //in child proc
                    close (pipefd[0]); // child doesnt read
                    dup2(pipefd[1],1); // set output to the pipe write
                    char* myargs[3] = {"ls", secondInput, NULL};
                    execvp(myargs[0],myargs); // execute ls command

                }else {
                    // in parent proc
                    pid = fork(); // fork again so we dont use parent to execute command
                    if (pid == 0) {
                        close(pipefd[1]); // parent doesn't write
                        dup2(pipefd[0], 0); //set input as the output from child
                        char *myargs[3] = {"grep", fifthInput, NULL}; // use grep command with specified word
                        execvp(myargs[0], myargs); //execute grep command
                    }
                    else{
                        wait(NULL); //wait for grep child
                    }
                    close(pipefd[0]); // close the pipe read
                    close(pipefd[1]); // close the pipe write
                    wait(NULL); // wait for ls child
                }
            }
        }

        // this is for ls -a dirName | grep grepWord
        if (strcmp(fourthInput, "|") == 0) {
            if (strcmp("grep", fifthInput) == 0) {
                int pipefd[2]; // create an array which is used as our pipe
                int pid;

                pipe (pipefd); // set up the pipe
                pid=fork(); // make child process

                if(pid<0){ // fork error
                    perror("ERROR");
                    exit(0);
                }
                else if (pid==0)
                {
                    //in child proc
                    close (pipefd[0]); // child doesnt read
                    dup2(pipefd[1],1); // set output to the pipe write
                    char* myargs[4] = {"ls", secondInput, thirdInput, NULL};
                    execvp(myargs[0],myargs); // execute ls command

                }else {
                    // in parent proc
                    pid = fork(); // fork again so we dont use parent to execute command
                    if (pid == 0) {
                        close(pipefd[1]); // parent doesn't write
                        dup2(pipefd[0], 0); //set input as the output from child
                        char *myargs[3] = {"grep", sixthInput, NULL}; // use grep command with specified word
                        execvp(myargs[0], myargs); //execute grep command
                    }
                    else{
                        wait(NULL); //wait for grep child
                    }
                    close(pipefd[0]); // close the pipe read
                    close(pipefd[1]); // close the pipe write
                    wait(NULL); // wait for ls child
                }
            }
        }
    }
}


/**
 * makes directory
 */
void mkdirCommand(char* secondInput){
    // make temporary path and check if exist
    char *tempPath = malloc((strlen(pwd) + strlen(secondInput) + strlen(slash)) * sizeof(char));
    arrayCleaner(tempPath,strlen(tempPath)/sizeof(char));
    strcpy(tempPath,pwd);
    strcat(tempPath, slash);
    strcat(tempPath, secondInput);
    //if it doesnt exist make directory
    if (isDirectoryExists(tempPath) == 0) {
        mkdir(tempPath,777);
        free(tempPath); // remember to free heap
    } else {
        printf("Directory already exists\n");
        free(tempPath);// remember to free heap
    }
}

/**
 * removes directory
 */
void rmdirCommand(char* secondInput){
    // make temporary path and check if exist
    char *tempPath = malloc((strlen(pwd) + strlen(secondInput) + strlen(slash)) * sizeof(char));
    arrayCleaner(tempPath,strlen(tempPath)/sizeof(char));
    strcpy(tempPath,pwd);
    strcat(tempPath, slash);
    strcat(tempPath, secondInput);
    //if it exist remove directory
    if (isDirectoryExists(tempPath) != 0) {
        rmdir(tempPath);
        free(tempPath);// remember to free heap
    } else {
        printf("Directory doesn't exists\n");
        free(tempPath);// remember to free heap
    }
}

/**
 * https://stackoverflow.com/questions/13450809/how-to-search-a-string-in-a-char-array-in-c
 * use strstr() to search for word in and output if found
 */
void grepCommand(char* secondInput, char* thirdInput){
    int pid = fork(); // make child process
    if (pid < 0){ // fork error
        perror("Error: ");
        exit(0);
    }
    else if (pid == 0) {
        char *myargs[4] = {"grep", secondInput, thirdInput,NULL}; //prepare command arguments
        execvp(myargs[0],myargs); //execute command
    }
    else{
        wait(NULL); // wait for child process
    }


}


/**
 * cat will output the content of a file given that the file exists
 * max number of characters on a line is set to 256
 * Pipeline is placed here also for grepping word in cat
 */
void catCommand(char* secondInput, char* fourthInput, char* fifthInput){
    if (isIODir == 1){
        // inspiration from http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
        int pid = fork(); // create child process
        if (pid < 0){// fork error
            perror("ERROR");
            exit(0);
        }
        else if (pid > 0){
            wait(NULL); // wait for child process to finish
        } else{
            // open the file with write only to use in dup2
            int out = open(fourthInput,O_WRONLY);
            // if file doesn't exists create it
            if (out == -1){
                creat(fourthInput,0777);
                out = open(fourthInput,O_WRONLY);
            }
            // set the STDOUT to point to the file
            dup2(out,STDOUT_FILENO);
            // close file open cause we already duped it
            close(out);
            // cat arguments
            char* myargs[3];
            myargs[0] = "cat";
            myargs[1] = secondInput;
            myargs[2] = NULL;
            execvp(myargs[0],myargs); // execute argument which will output to the file
        }
    }

    else if (isPipe == 0) {
        int pid = fork(); // create child process
        if (pid < 0){
            perror("ERROR");
            exit(0);
        }
        else if (pid > 0){
            wait(NULL); // wait for child process to finish
        } else{
            char* myargs[3];
            myargs[0] = "cat";
            myargs[1] = secondInput;
            myargs[2] = NULL;
            execvp(myargs[0],myargs); // execute arguments
        }
    }

    else {
        // if grep is in cat
        if (strcmp("grep", fourthInput) == 0) {
            int pipefd[2]; // create an array which is used as our pipe
            int pid;

            pipe (pipefd); // set up the pipe
            pid=fork(); // make child process

            if(pid<0){ // fork error
                perror("ERROR");
                exit(0);
            }
            else if (pid==0)
            {
                //in child proc
                close (pipefd[0]); // child doesnt read
                dup2(pipefd[1],1); // set output to the pipe write
                char* myargs[3] = {"cat",secondInput, NULL};
                execvp(myargs[0],myargs); // execute cat command

            }else {
                // in parent proc
                pid = fork(); // fork again so we dont use parent to execute command
                if (pid == 0) {
                    close(pipefd[1]); // parent doesn't write
                    dup2(pipefd[0], 0); //set input as the output from child
                    char *myargs[3] = {"grep", fifthInput, NULL}; // use grep command with specified word
                    execvp(myargs[0], myargs); //execute grep command
                    close(pipefd[0]); // close the pipe read
                }
                else{
                    wait(NULL); //wait for grep child
                }
                close(pipefd[0]); // close the pipe read
                close(pipefd[1]); // close the pipe write
                wait(NULL); // wait for cat child
            }
        }
    }
}






int main() {
    /**
     * Declare some array pointers to use later with malloc, and a max sized input array
     */
    char *firstInput = "";
    char *secondInput = "";
    char *thirdInput = "";
    char *fourthInput = "";
    char *fifthInput = "";
    char *sixthInput = "";
    char input[256] = "";

    slash[0] = '/';
    /**
     * Gets name of current path
     */
    getcwd(pwd,100);


    while (TRUE) {
        // set pipe and IO direction to false
        isPipe = 0;
        isIODir = 0;
        // clean the input array
        arrayCleaner(input, sizeof(input));
        /**
         * Show current path + $ in the console
         */
        printf("%s $\t", pwd);


        /**
         * Read input of whole line.
         * Instead of scanf() cause i had problems  https://www.quora.com/How-do-I-read-and-display-a-full-sentence-in-C
         */
        scanf("%[^\n]%*c", input);


        /**
         * Check for pipe and IO redirection
         */
        for (int j = 0; j < strlen(input); ++j) {
            if (input[j] == '|') {
                isPipe = 1;
            }
            else if (input[j] == '>'){
                isIODir = 1;
            }
        }


        /**
         * Splits input into arrays
         */
        firstInput = inputSplitter(input, 1);
        secondInput = inputSplitter(input, 2);
        thirdInput = inputSplitter(input, 3);
        fourthInput = inputSplitter(input,4);
        fifthInput = inputSplitter(input,5);
        sixthInput = inputSplitter(input,6);



        // check what command was written and execute the specified command
        if (strcmp("cd", firstInput) == 0) {
            cdCommand(secondInput);
        }

        else if (strcmp("ls", firstInput) == 0) {
            lsCommand(secondInput,thirdInput,fourthInput,fifthInput,sixthInput);
        }

            /**
             * will clear console but only through command line
             * https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c
             */
        else if (strcmp("clear", firstInput) == 0) {
            system("clear\n");
        }

        else if (strcmp("pwd", firstInput) == 0){
            printf("%s\n",pwd);
        }

        else if (strcmp("mkdir", firstInput) == 0){
            mkdirCommand(secondInput);
        }

        else if (strcmp("rmdir", firstInput) == 0) {
            rmdirCommand(secondInput);
        }

        else if (strcmp("grep", firstInput) == 0) {
            grepCommand(secondInput,thirdInput);
        }

        else if (strcmp("cat", firstInput) == 0) {
            catCommand(secondInput, fourthInput,fifthInput);
        }

        else if (strcmp("exit",firstInput) == 0){
            exit(0);
        }

        else {
            printf("Command not found\n");
        }



        /**
         * Free arrays for next loop and flush input from keyboard
         */
        free(firstInput);
        free(secondInput);
        free(thirdInput);
        free(fourthInput);
        free(fifthInput);
        free(sixthInput);
        fflush(stdin);
    }
    return 0;
}
