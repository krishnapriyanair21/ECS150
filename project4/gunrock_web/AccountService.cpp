#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AccountService.h"
#include "ClientError.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

// Error-checking:
// user_id doesn't exist in the users database
// user_id doesn't match the authenticated user
// missing email argument
// email is in the wrong format?
AccountService::AccountService() : HttpService("/users") {
  
}

void AccountService::get(HTTPRequest *request, HTTPResponse *response) {
    
}

void AccountService::put(HTTPRequest *request, HTTPResponse *response) {
    // Updates the information for a user
    // given email
    // response = email and balance

    // WwwFormEncodedDict fullRequest = request->formEncodedBody();
    // string email = fullRequest.get("email");

}
