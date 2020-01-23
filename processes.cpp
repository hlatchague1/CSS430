#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

//values 0, 1 respectively
enum {READ, WRITE};

int main(int argc, char** argv){

    int returnPid;

    //pipe between grandchild & great-grandchild 
    int pipeFD1[2];
    //pipe between child & grandchild
    int pipeFD2[2];

    if (argc != 2){
        cerr << "Invalid number of arguments, try again!" << endl;
        exit(-1);
    }

    if (pipe(pipeFD1) < 0){
        cerr << "Error: pipe (child to grandchild)" << endl;
        exit(-1);
    }

    if (pipe(pipeFD2) < 0){
        cerr << "Error: pipe (grandchild to great-grandchild)" << endl;
        exit(-1);
    }

    if ((returnPid = fork()) < 0){
        cerr << "Error: fork (parent)" << endl;
        exit(-1);
    }

    //great-grandchild
    if (returnPid == 0){
        if ((returnPid = fork()) < 0){
        cerr << "Error: fork (child)" << endl;
        exit(-1);
        }

        //grandchild
        if (returnPid == 0){ // grandchild
            if ((returnPid = fork()) < 0){
            cerr << "Error: fork (grandchild)" << endl;
            exit(-1);
            }

            //child
            if (returnPid == 0){
                close(pipeFD1[WRITE]);
                close(pipeFD1[READ]);
                close(pipeFD2[WRITE]);
                dup2(pipeFD2[READ], READ);
                execlp("wc", "wc", "-l" , NULL);
                
            }
            //grandchild
            else{
                close(pipeFD1[WRITE]);
                close(pipeFD2[READ]);
                dup2(pipeFD1[READ], READ);                
                dup2(pipeFD2[WRITE], WRITE);
                execlp("grep", "grep", argv[1] , NULL);

            }
        }
        //great-grandchild
        else{
            close(pipeFD2[READ]);
            close(pipeFD2[WRITE]);
            close(pipeFD1[READ]);
            dup2(pipeFD1[WRITE], WRITE);
            execlp("ps", "ps", "-A", NULL);

        }
    }
    //parent
    else{
        wait(NULL);
    } 
    return 0; 
}

// redirect output by doing: Command > file.txt (creates file)
// append to file: Command >> file.txt (adds to file)