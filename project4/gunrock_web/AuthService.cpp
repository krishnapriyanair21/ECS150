#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AuthService.h"
#include "StringUtils.h"
#include "ClientError.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

void rapidJSONResponse(string currUserToken, string currUserID, HTTPResponse *response);
int errorChecks(string username, string password, HTTPResponse *response, User *user = NULL);

AuthService::AuthService() : HttpService("/auth-tokens") {
  
}

void AuthService::post(HTTPRequest *request, HTTPResponse *response) {
    User *userExists = getAuthenticatedUser(request);
    string currUserID; 
    string currUserToken;
    WwwFormEncodedDict fullRequest;
    string username;
    string password;
    int error = 0;
    
    // get username and password
    fullRequest = request->formEncodedBody();
    username = fullRequest.get("username");
    password = fullRequest.get("password");
    if (userExists == NULL){
        User *newUser = new User;
        newUser->user_id = StringUtils::createUserId();
        // add to User obj
        newUser->username = username;
        newUser->password = password;
        cout << "username / password in post: " << username << " / " <<password<<endl;
        // error checking
        error = errorChecks(username, password, response);
        if (error!= 0){ 
            response->setStatus(error); 
        } else {
            // create response object
            currUserID = newUser->user_id;
            currUserToken = StringUtils::createAuthToken();

            // push to database 
            m_db->auth_tokens.insert(std::pair <string, User*>(currUserToken, newUser));
            m_db->users.insert(std::pair <string, User*>(username, newUser));

            //response object
            rapidJSONResponse(currUserToken, currUserID, response);
            response->setStatus(201);
        }
    }
    else{
        // existing user
        currUserID = userExists->user_id;
        // if (request->hasAuthToken()){
        //     currUserToken = request->getAuthToken();
        // }else{
            currUserToken = StringUtils::createAuthToken();
        // }
        // Error checks
        error = errorChecks(username, password, response, userExists);
        if (error != 0) { 
            response->setStatus(error); 
        }else {
            // push to database 
            m_db->auth_tokens.insert(std::pair <string, User*>(currUserToken, userExists));
            // response object
            rapidJSONResponse(currUserToken, currUserID, response);
            response->setStatus(200);
        }
    }
}

/// Not Tested 
void AuthService::del(HTTPRequest *request, HTTPResponse *response) {
    std::map<string,User*>::iterator it;
    string deleteAuthToken;
    // string checkAuthToken;
    User *deleteUser;
    User *checkUser;
    if (request->hasAuthToken()){
        vector<string> path = request->getPathComponents();
        deleteAuthToken = path.back(); // last in parameter list
        // // check if user_id is correct
        checkUser = getAuthenticatedUser(request);
        for(it = m_db->auth_tokens.begin(); it != m_db->auth_tokens.end(); ++it){ // loop through database
            if (m_db->auth_tokens.find(deleteAuthToken) != m_db->users.end()){
                deleteUser = it -> second;
            }
        }
        if (checkUser == deleteUser){
            m_db->auth_tokens.erase(deleteAuthToken);
            response->setStatus(200);
        }else{
            response->setStatus(400);
            throw ClientError::badRequest();
        }
    //     // use getAuthenticatedUser
        // for(it = m_db->auth_tokens.begin(); it != m_db->auth_tokens.end(); ++it){ // loop through database
        //     if (m_db->users.find(deleteAuthToken) != m_db->users.end()){
        //         deleteUser = it ->second;
        //     }
    //         if (m_db->users.find(checkAuthToken) != m_db->users.end()){
    //             checkUser = it ->second;
    //         }
    //     }
    //     // check User from this token and deleteAuthToken have same id (if yes DELETE)
    //     cout << checkUser->user_id << " check user ID" <<endl;
    //     cout << deleteUser->user_id << " delete USer ID" <<endl;
    //     if (checkUser->user_id == deleteUser->user_id){
    //         m_db->auth_tokens.erase(deleteAuthToken);
    //         response->setStatus(200);
    //     }else{
    //         response->setStatus(400);
    //     }
    }else{
        throw ClientError::badRequest(); // no token in delete call
    }
}

void rapidJSONResponse(string currUserToken, string currUserID, HTTPResponse *response){
    // use rapidjson to create a return object
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();

    // add a key value pair directly to the object
    o.AddMember("auth_token", currUserToken, a);
    o.AddMember("user_id", currUserID, a);

    // now some rapidjson boilerplate for converting the JSON object to a string
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    // set the return object
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}

int errorChecks(string username, string password, HTTPResponse *response, User *user){
    // missing arguements
    if ((username == "") || (password == "")){
        throw ClientError::badRequest();
        return 400;
    }
    // username lowercase
    unsigned size = username.size();
    char tempUsername[size]; 
    strcpy(tempUsername, username.c_str());
    for (unsigned i = 0; i < size; i++){
        if(!islower(tempUsername[i])){
            throw ClientError::badRequest();
            return 400;
        }
    }
    // password in database
    if (user){
        if (user->password != password){
            throw ClientError::forbidden();
            return 403;
        }
    }
    return 0; // no error
}