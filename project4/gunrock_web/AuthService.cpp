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

// Error checking:
// missing username or password arguments
// username is not all lowercase
// password doesn't match user in the users database
AuthService::AuthService() : HttpService("/auth-tokens") {
  
}

void AuthService::post(HTTPRequest *request, HTTPResponse *response) {
    User *userExists = getAuthenticatedUser(request);
    if (userExists == NULL){
        User *newUser = new User;
        newUser->user_id = StringUtils::createUserId();
        
        newUser->username = request->getAuthToken();
        // newUser->password = request->getParams(); // change
        /// if user does not exist, create user?
        response->setStatus(201);
    }
    /// set status to 201
}

void AuthService::del(HTTPRequest *request, HTTPResponse *response) {
    if (request->)
    // check head token == user token
    // find URL token USer pointer in database
    // compare to getAutheticatedUser
    // if match delete URL pointer 
}
