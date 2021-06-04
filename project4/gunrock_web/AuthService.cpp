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

// Error checking:
// missing username or password arguments
// username is not all lowercase
// password doesn't match user in the users database
AuthService::AuthService() : HttpService("/auth-tokens") {
  
}

void AuthService::post(HTTPRequest *request, HTTPResponse *response) {
    User *userExists = getAuthenticatedUser(request);
    string currUserID; 
    string currUserToken;
    WwwFormEncodedDict fullRequest;
    string username;
    string password;
    

    if (userExists == NULL){
        User *newUser = new User;
        newUser->user_id = StringUtils::createUserId();
        // get username and password
        fullRequest = request->formEncodedBody();
        username = fullRequest.get("username");
        password = fullRequest.get("password");

        // add to User obj
        newUser->username = username;
        newUser->password = password;

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
    else{
        // existing user
        currUserID = userExists->user_id;
        currUserToken = request->getAuthToken();
        rapidJSONResponse(currUserToken, currUserID, response);
        response->setStatus(200);
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
