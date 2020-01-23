#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv){

    enum {READ, WRITE};

    int returnPid;

    int childToGrandPipeFD[2];
    int grandToGreatPipeFD[2];
    
    int childToGrandRC = pipe(childToGrandPipeFD);
    int grandToGreatRC = pipe(grandToGreatPipeFD);

    if (argc != 2){
        cerr << "Invalid number of arguments, try again!" << endl;
        exit(-1);
    }

    if (childToGrandRC < 0){
        cerr << "Error: pipe (child to grandchild)" << endl;
        exit(-1);
    }

    if (grandToGreatRC < 0){
        cerr << "Error: pipe (grandchild to great-grandchild)" << endl;
        exit(-1);
    }

    if (returnPid = fork() < 0){
        cerr << "Error: fork (parent)" << endl;
        exit(-1);
    }

    else if (returnPid == 0){ // child
        cout << "Child returnPid: " << returnPid << endl;

        if (returnPid = fork() < 0){
        cerr << "Error: fork (child)" << endl;
        exit(-1);
        }

        else if (returnPid == 0){ // grandchild
            cout << "Grandchild returnPid: " << returnPid << endl;

            if (returnPid = fork() < 0){
            cerr << "Error: fork (grandchild)" << endl;
            exit(-1);
            }

            else if (returnPid == 0){ // great-grandchild
                cout << "Great-grandchild returnPid: " << returnPid << endl;
                if (returnPid = fork() < 0){
                cerr << "Error: fork (great-grandchild)" << endl;
                exit(-1);
                }
                close(grandToGreatPipeFD[READ]);
                close(childToGrandPipeFD[WRITE]);
                close(childToGrandPipeFD[READ]);
                dup2(grandToGreatPipeFD[WRITE], WRITE);
                execlp("ps", "ps", "-A", NULL);
            }
            else{ // grandchild
                close(grandToGreatPipeFD[WRITE]);
                dup2(grandToGreatPipeFD[READ], READ);
                close(childToGrandPipeFD[READ]);
                dup2(childToGrandPipeFD[WRITE], WRITE);
                execlp("grep", "grep", argv[1] , NULL);

            }
        }
        else{ // child
            close(grandToGreatPipeFD[WRITE]);
            close(grandToGreatPipeFD[READ]);
            close(childToGrandPipeFD[WRITE]);
            dup2(childToGrandPipeFD[READ], READ);
            execlp("wc", "wc", "-l" , NULL);

        }
    }
    else{ // parent
        wait(NULL);
    } 
    return 0; 
};

// redirect output by doing: Command > file.txt (creates file)
// append to file: Command >> file.txt (adds to file)