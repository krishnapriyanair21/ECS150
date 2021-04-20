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
#include <sys/uio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

using namespace std;

vector<string> parse(string rawInput);
void printError();
void executeCmd(vector<string> parsedInput, vector<string> &path);
void pathFunction(vector<string> parsedInput, vector<string> &path);
void executeBuiltIn (vector<string> parsedInput, vector<string> &path);
int checkMode(vector<string> parsedInput);
int redirectSyntaxCheck(vector<string> parsedInput);
void redirect(vector<string> parsedInput, vector<string> path);
vector<string> addSpaces(vector<string> parsedInput);
void parallel (vector<string> parsedInput, vector<string> path);
vector<string> removeAmps(vector<string> parsedInput);
void redirectAndParallel (vector<string> parsedInput, vector<string> &path);

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
                        checkMode(parsedInput);
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
    vector<string> tempInput;
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
        int type = checkMode(parsedInput);
        // redirection parallel or normal 
        if (type == 0){ // normal
            pid_t ret = fork();
            if (ret == 0){
                executeBuiltIn(parsedInput, path);
            } else{
                wait(NULL);
            }
        }else if (type == 1){ // redirect
            string currCmd = parsedInput[parsedInput.size() - 1];
            if (currCmd[currCmd.length() - 1] == '>'){
                printError();
                return;
            }
            if (redirectSyntaxCheck(parsedInput) == -1){ // make sure redirect is formatted correct
                printError();
                return;
            }
            tempInput = addSpaces(parsedInput);
            redirect(tempInput, path);
        } else if (type == 2){ // parallel
            if (parsedInput[0] == "&") // only &
                return;
            tempInput = addSpaces(parsedInput);
            tempInput = removeAmps(tempInput);
            parallel(tempInput, path);
        }else if (type == 3){ // para + redirect
            redirectAndParallel(parsedInput, path);
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
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
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

int checkMode(vector<string> parsedInput){
    // normal = 0, redirect = 1, parallel = 2, redirect + para = 3
    int amps = 0; // ampersand
    int arrow = 0; // redirect

    for(unsigned int i = 0; i < parsedInput.size(); i++){
        string currString = parsedInput[i];
        for(unsigned int j = 0; j < currString.length(); j++){
            if (currString[j] == '>')
                arrow++;
            if (currString[j] == '&')
                amps++;
        }
    }
    if (arrow > 0 && amps > 0) // redirect + para
        return 3;
    else if (amps > 0) // parallel
        return 2;
    else if (arrow > 0) // redirect
        return 1;
    else // normal
        return 0;
}

/* Edge case checks*/
int redirectSyntaxCheck(vector<string> parsedInput){
    int output = 0;
    int arrow = 0;
    for (unsigned int i = 0; i < parsedInput.size(); i++){
        if (parsedInput[i] == ">"){
            arrow++;
            if (parsedInput.size() - i > 1)
                output = -1;
        }
    }
    if (arrow > 1)
        output = -1;
    return output;
}

/* All redirection code */
void redirect(vector<string> parsedInput, vector<string> path){
    vector<string> inputCmd;
    string outputFileTemp;
    char outputFile[4096];
    for (unsigned int i = 0; i < parsedInput.size(); i++){
        string curr = parsedInput[i];
        if (curr == ">"){
            outputFileTemp = parsedInput[i + 1];
            break;
        }
        else
            inputCmd.push_back(curr);
    }
    strcpy(outputFile, outputFileTemp.c_str());
    pid_t ret = fork();
    if (ret == 0){
        int fd = open(outputFile, O_RDWR | O_TRUNC | O_CREAT, 0644);
        if (fd < 0){ // error check
            cerr<< "couldn't open file" <<endl;
        }
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        executeBuiltIn(inputCmd, path);
    }else{
        wait(NULL);
    }
}

/* Add spaces to input where necessary */
vector<string> addSpaces(vector<string> parsedInput){
    vector<string> inputToSendBack;
    string temp= "";
    bool check = false;
    string tempWithArrow = "";
    string tempWithAmps= "";
    for (unsigned int i = 0; i < parsedInput.size(); i++){
        string curr = parsedInput[i];
        for (unsigned int j = 0; j < curr.length(); j++){
            if (curr[j] == '>'){
                if (curr[j - 1] != ' ' && curr[j + 1] != ' '){
                    tempWithArrow = " > ";
                    check = true;
                }
                else if (curr[j - 1] != ' '){
                    tempWithArrow = " >";
                    check = true;
                }
                else if(curr[j + 1] != ' '){
                    tempWithArrow = "> ";
                    check = true;
                }
                temp.append(tempWithArrow); 
                tempWithArrow = "";
            }
            if (curr[j] == '&'){
                if (j == curr.length() - 1){
                    tempWithAmps = "";
                    check = true;
                }
                else if (curr[j - 1] != ' ' && curr[j + 1] != ' '){
                    tempWithAmps = " & ";
                    check = true;
                }
                else if (curr[j - 1] != ' '){
                    tempWithAmps = " &";
                    check = true;
                }
                else if (curr[j + 1] != ' '){
                    tempWithAmps = "& ";
                    check = true;
                }
                temp.append(tempWithAmps);
                tempWithAmps = "";
            }
            if (!check){
                temp.push_back(curr[j]); // for char use push back for string use append
            }
            check = false;
            if (j == curr.length() - 1 && i != parsedInput.size() - 1){ // add space at the end
                temp.push_back(' ');
            }
        }
        inputToSendBack.push_back(temp); 
        temp = "";
    }
    for (unsigned int i = 0; i < inputToSendBack.size(); i++){ // reparse input
        temp = temp + inputToSendBack[i];
    }
    inputToSendBack = parse(temp);
    return inputToSendBack;
}

void parallel (vector<string> parsedInput, vector<string> path){
    vector<string> inputCmd;
    unsigned int size = parsedInput.size();
    for (unsigned int i = 0; i < parsedInput.size(); i++){
        inputCmd.clear();
        inputCmd.push_back(parsedInput[i]);
        pid_t ret = fork();
        if (ret == 0){
            executeBuiltIn(inputCmd, path);
        }
    }
    while(size != 0){
        wait(NULL);
        size--;
    }
}

vector<string> removeAmps(vector<string> parsedInput){
    vector<string> returnVal;
    for (unsigned int i = 0; i < parsedInput.size(); i++){
        if (parsedInput[i] != "&"){
            returnVal.push_back(parsedInput[i]);
        }
    }
    return returnVal;
}
void redirectAndParallel (vector<string> parsedInput, vector<string> &path){
    vector<string> inputCmd;
    string outputFileTemp = "";
    char outputFile[4096];
    for (unsigned int i = 0; i < parsedInput.size(); i++){
            if (parsedInput[i] == ">"){
                outputFileTemp = parsedInput[i + 1];
                break;
            }
            else if (parsedInput[i] == "&"){
                strcpy(outputFile, outputFileTemp.c_str());
                pid_t ret = fork();
                if (ret == 0){
                    int fd = open(outputFile, O_RDWR | O_TRUNC | O_CREAT, 0644 );
                    if (fd < 0){ // error check
                        cerr<< "couldn't open file" <<endl;
                    }
                    dup2(fd, 1);
                    dup2(fd, 2);
                    close(fd);
                    executeBuiltIn(inputCmd, path);
                }else{
                    wait(NULL);
                }
            }
            else{
                inputCmd.push_back(parsedInput[i]);
            }
    }
    redirect(inputCmd, path);
}
