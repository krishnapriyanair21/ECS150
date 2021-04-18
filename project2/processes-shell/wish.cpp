/* This project was done with Dave Khalasi and Divya Singh */

#include <cstring>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/wait.h>

using namespace std;

vector<string> parse(string rawInput);
void printError();
void executeCmd(vector<string> parsedInput, vector<string> &path);
void pathFunction(vector<string> parsedInput, vector<string> &path);
void executeBuiltIn (vector<string> parsedInput, vector<string> &path);

int main (int argc, char *argv[]){

    vector<string> parsedInput;
    string rawInput;
    vector<string> curPath;
    curPath.push_back("");
    curPath.push_back("/bin/");

    if (argc == 1){ // interactive
        cout << "wish> ";
        while (getline(cin, rawInput)){ 
            parsedInput = parse(rawInput);
            executeCmd(parsedInput, curPath);
            cout<<"wish> ";
        }
    }else if (argc > 1){ // batch
        if (argc > 3){  // more than 1 file
            printError();
        }
        for (int i = 1; i < argc; i++){
            ifstream inputFile;
            inputFile.open(argv[i]);
            if (!inputFile){
                printError();
                exit(1);
            }  
            else{
                while (getline(inputFile, rawInput)){
                    parsedInput = parse(rawInput);
                    if (parsedInput.size() > 0){
                        executeCmd(parsedInput,curPath);
                    }
                }
            }
            inputFile.close();
        }
    }
    return 0;
}

void executeCmd(vector<string> parsedInput, vector<string> &path){
        if (parsedInput[0] == "exit"){  
            if (parsedInput.size() > 1){
                printError(); 
            }else{
                exit(0);
            }
        } else if (parsedInput[0] == "cd"){
            if (parsedInput.size() < 2 ){
                printError(); 
            }else{
                string convertString = parsedInput[1];
                int destSize = convertString.size();
                char dirToChange[destSize + 1];
                strcpy(dirToChange, convertString.c_str()); // need char array for chdir()
                if (!(chdir(dirToChange) == 0)){
                    printError();
                }     
            }
        } else if (parsedInput[0] == "path") {
           pathFunction(parsedInput, path);
        }else{
            pid_t ret = fork();
            if (ret == 0){
                // redirection parallel or normal 
                executeBuiltIn(parsedInput, path);
            } else{
                wait(NULL);
            }
        }
    return;
}

void pathFunction(vector<string> parsedInput, vector<string> &path){
    path.clear();
    path.push_back("");

    for (unsigned int i = 1; i < parsedInput.size(); i++){
        if (parsedInput[i][0] != '/') 
            parsedInput[i] = "/" + parsedInput [i];
        if (parsedInput[i][parsedInput.size() - 1] != '/')
            parsedInput[i] += "/";
        char curDir[4096];
        getcwd(curDir, 4096);
        parsedInput[i] = curDir + parsedInput[i];
        path.push_back(parsedInput[i]);
    }
}

vector<string> parse(string rawInput){  // stack overflow
    istringstream s(rawInput);
    vector<string> parsedInput;
    string cmdx; // command at x 
    while(s >> cmdx){
        parsedInput.push_back(cmdx);
    }
    return parsedInput;
}

void printError(){
    cerr<<"An error has occurred"<<endl;
}

void executeBuiltIn (vector<string> parsedInput, vector<string> &path){
    char *arg[parsedInput.size() + 1];
    for (unsigned int j = 0; j < parsedInput.size(); j++){
        arg[j] = new char[parsedInput[j].size() + 1];
        strcpy(arg[j], parsedInput[j].c_str());
    }
    arg[parsedInput.size()] = nullptr;

    for (unsigned int j = 0; j < path.size(); j++){
        char newCmd[path[j].size() + parsedInput.size() + 1];
        strcpy(newCmd, (path[j] + parsedInput[0]).c_str());
        execv(newCmd, arg);
    }
    printError();
    exit(1); // cannot execute built in 
}