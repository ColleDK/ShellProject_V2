#include <fcntl.h>
#include "main.h"

/**
 * Global variables and functions
 */
int TRUE = 1;
int isPipe = 0;
int isIODir = 0;
char slash[1];
char pwd[100];
void grepCommand(); //i need to declare this because i use it in a function before the real declaration


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
    arrayCleaner(array, sizeof(array)/sizeof(char));
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
    if (strcmp("~",secondInput) == 0 || strcmp("",secondInput) == 0){
        chdir(getenv("HOME"));
        getcwd(pwd,100);
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
        int pid = fork();
        if (pid > 0) {
            wait(NULL);
        } else {
            char *myargs[4];
            myargs[0] = strdup("ls");
            if (strcmp("", secondInput) != 0) {
                myargs[1] = strdup(secondInput);
            } else {
                myargs[1] = NULL;
            }
            if (strcmp("", thirdInput) != 0) {
                myargs[2] = strdup(thirdInput);
            } else { myargs[2] = NULL; }
            myargs[3] = NULL;

            execvp(myargs[0], myargs);
        }
    }

        // MAKE LATER
    else {
        // if pipe is enabled (only grep is allowed in program) then get name and send to grepcommand
        if (strcmp(secondInput, "|") == 0) {

        }
    }
}


/**
 * makes directory
 */
void mkdirCommand(char* secondInput){
    // make temporary path and check if exist
    char *tempPath = malloc((strlen(pwd) + strlen(secondInput) + strlen(slash)) * sizeof(char));
    arrayCleaner(tempPath,sizeof(tempPath)/sizeof(char));
    strcpy(tempPath,pwd);
    strcat(tempPath, slash);
    strcat(tempPath, secondInput);
    //if it doesnt exist make directory
    if (isDirectoryExists(tempPath) == 0) {
        mkdir(tempPath,777);
        free(tempPath);
    } else {
        printf("Directory already exists\n");
        free(tempPath);
    }
}

/**
 * removes directory
 */
void rmdirCommand(char* secondInput){
    // make temporary path and check if exist
    char *tempPath = malloc((strlen(pwd) + strlen(secondInput) + strlen(slash)) * sizeof(char));
    arrayCleaner(tempPath,sizeof(tempPath)/sizeof(char));
    strcpy(tempPath,pwd);
    strcat(tempPath, slash);
    strcat(tempPath, secondInput);
    //if it exist remove directory
    if (isDirectoryExists(tempPath) != 0) {
        rmdir(tempPath);
        free(tempPath);
    } else {
        printf("Directory doesn't exists\n");
        free(tempPath);
    }
}

/**
 * https://stackoverflow.com/questions/13450809/how-to-search-a-string-in-a-char-array-in-c
 * use strstr() to search for word in and output if found
 */
void grepCommand(char* secondInput, char* thirdInput, int lsEnabled, char lsName[], char grepName[]){
    //check if it came from lscommand
    if (lsEnabled == 0) {
        // make temporary path to the file
        char *tempPath = malloc((strlen(pwd) + strlen(thirdInput) + strlen(slash)) * sizeof(char));
        strcpy(tempPath, pwd);
        strcat(tempPath, thirdInput);
        // give pointer read permissions
        FILE *f = fopen(tempPath, "r");
        // check if file exists
        if (f != NULL) {
            char fileArr[256] = "";
            arrayCleaner(fileArr, sizeof(fileArr));
            char *string;
            // get 1 line at a time
            while (fgets(fileArr, 256, f) != NULL) {
                // built in function to check if letter or word is in another char[]
                string = strstr(fileArr, secondInput);
                // if grepped word is found in line then print out line
                if (string != NULL) {
                    printf("%s", fileArr);
                }
            }
        } else {
            printf("No file found\n");
        }
        fclose(f);
        free(tempPath);
    }
    else{
        // maybe pointless but check if pointer is non empty
        if (lsName != NULL){
            char* string;
            // check if grep word is in name of ls file/directory
            string = strstr(lsName,grepName);
            if (string != NULL){
                printf("%s\n", lsName);
            }
        }
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
        int status;
        int pid = fork();
        if (pid > 0){
            waitpid(pid,&status,0);
        } else{
            // open the file with write only to use in dup2
            int out = open(fourthInput,O_WRONLY);
            // if file doesn't exists create it
            if (out == -1){
                creat(fourthInput,0777);
                out = open(fourthInput,O_WRONLY);
            }
            dup2(out,STDOUT_FILENO);
            // close file open cause we already duped it
            close(out);
            // cat arguments
            char* myargs[3];
            myargs[0] = "cat";
            myargs[1] = secondInput;
            myargs[2] = NULL;
            execvp(myargs[0],myargs);
        }
    }

    else if (isPipe == 0) {
        // give pointer read permissions
        int status;
        int pid = fork();
        if (pid > 0){
            waitpid(pid,&status,0);
        } else{
            char* myargs[3];
            myargs[0] = "cat";
            myargs[1] = secondInput;
            myargs[2] = NULL;
            execvp(myargs[0],myargs);
        }
    }
    else {
        // if grep is in cat
        if (strcmp("grep", fourthInput) == 0) {
            int pipefd[2];
            int pid;
            char recv[256];

            pipe (pipefd);

            pid=fork();

            if(pid<0){
                perror("fork");
                exit(1);
            }
            else if (pid==0)
            {
                //in child proc
                close (pipefd[0]);
                dup2(pipefd[1],1);
                char* myargs[3] = {"cat",secondInput, NULL};
                execvp(myargs[0],myargs);

            }else
            {
                close(pipefd[1]); // parent doesn't write

                // https://stackoverflow.com/questions/10700982/redirecting-stdout-to-pipe-in-c
                while(read(pipefd[0], recv, 256) > 0)
                {
//                    char* myargs[3] = {"grep", fifthInput, NULL};
//                    execvp(myargs[0],myargs);
                    write(1, recv, 256); // fd = 1 -> stdout
                }
                close(pipefd[0]);
                wait(NULL);
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
        isPipe = 0;
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
         * Check for pipe
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

            /**
             * Echoes everything after echo and space
             */
        else if (strcmp("echo", firstInput) == 0) {
            for (int i = 5; i < strlen(input); ++i) {
                printf("%c", input[i]);
            }
            printf("\n");
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
         * Clean the arrays for next loop and flush input from keyboard
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
