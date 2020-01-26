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
    int pipeGCtoGGC[2];
    //pipe between child & grandchild
    int pipeCtoGC[2];

    if (argc != 2){
        cerr << "Invalid number of arguments, try again!" << endl;
        exit(-1);
    }

    if (pipe(pipeGCtoGGC) < 0){
        cerr << "Error: pipe (child to grandchild)" << endl;
        exit(-1);
    }

    if (pipe(pipeCtoGC) < 0){
        cerr << "Error: pipe (grandchild to great-grandchild)" << endl;
        exit(-1);
    }

    if ((returnPid = fork()) < 0){
        cerr << "Error: fork" << endl;
        exit(-1);
    }

    if (returnPid == 0){
        if ((returnPid = fork()) < 0){
        cerr << "Error: fork" << endl;
        exit(-1);
        }

        if (returnPid == 0){
            if ((returnPid = fork()) < 0){
            cerr << "Error: fork" << endl;
            exit(-1);
            }

            //child
            if (returnPid == 0){
                close(pipeGCtoGGC[WRITE]);
                close(pipeGCtoGGC[READ]);
                close(pipeCtoGC[WRITE]);
                dup2(pipeCtoGC[READ], READ);
                int rc = execlp("wc", "wc", "-l" , NULL);
                if(rc < 0){
                    cerr << "Error with process: wc -l " << endl;
                }                           
            }
            //grandchild
            else{
                close(pipeGCtoGGC[WRITE]);
                close(pipeCtoGC[READ]);
                dup2(pipeGCtoGGC[READ], READ);                
                dup2(pipeCtoGC[WRITE], WRITE);
                int rc = execlp("grep", "grep", argv[1] , NULL);
                if(rc < 0){
                    cerr << "Error with process: grep argv[1] " << endl;
                }
            }
        }
        //great-grandchild
        else{
            close(pipeCtoGC[READ]);
            close(pipeCtoGC[WRITE]);
            close(pipeGCtoGGC[READ]);
            dup2(pipeGCtoGGC[WRITE], WRITE);
            int rc = execlp("ps", "ps", "-A", NULL);
            if(rc < 0){
                    cerr << "Error with process: ps -A " << endl;
            }
        }
    }
    //parent
    else{
        wait(NULL);
    } 
    return 0; 
}