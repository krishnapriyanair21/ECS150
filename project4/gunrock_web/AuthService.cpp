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

// Error checking:
// missing username or password arguments
// username is not all lowercase
// password doesn't match user in the users database (setStatus->(403))
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
        if (request->hasAuthToken()){
            currUserToken = request->getAuthToken();
        }else{
            // pull actual auth_token then update currUserToken
            currUserToken = request->getBody();
        }
        // Error checks
        error = errorChecks(username, password, response, userExists);
        if (error != 0) { 
            response->setStatus(error); 
        }else {
            // response object
            rapidJSONResponse(currUserToken, currUserID, response);
            response->setStatus(200);
        }
    }
}

void AuthService::del(HTTPRequest *request, HTTPResponse *response) {
    // if (request->)
    // check head token == user token
    // find URL token USer pointer in database
    // compare to getAutheticatedUser
    // if match delete URL pointer 
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
        return 400;
    }
    // username lowercase
    unsigned size = username.size();
    char tempUsername[size]; 
    strcpy(tempUsername, username.c_str());
    for (unsigned i = 0; i < size; i++){
        if(!islower(tempUsername[i])){
            return 400;
        }
    }
    
    // password in database
    if (user){
        if (user->password != password){
            return 403;
        }
    }
    return 0; // no error
}