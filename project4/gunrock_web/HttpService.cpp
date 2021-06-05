#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <map>

#include "HttpService.h"
#include "ClientError.h"

using namespace std;

HttpService::HttpService(string pathPrefix) {
  this->m_pathPrefix = pathPrefix;
}

User *HttpService::getAuthenticatedUser(HTTPRequest *request)  {
  WwwFormEncodedDict fullRequest;
  string username;
  User *returnUser;
  string authToken;
  std::map<string, User*>::iterator iter;

  if(request->hasAuthToken()){  // with token
    authToken = request->getAuthToken();
    for(iter = m_db->auth_tokens.begin(); iter != m_db->auth_tokens.end(); ++iter){ // loop through database
      if (iter->first == authToken){ 
        returnUser = iter->second;
      }
    }
    return returnUser;
  }
  else{ // with username
    fullRequest = request->formEncodedBody();
    username = fullRequest.get("username");
    for(iter = m_db->users.begin(); iter != m_db->users.end(); ++iter){ // loop through database
      if (iter->first == username){
        returnUser = iter->second;
        return returnUser;
      }
    }
  }
  return NULL;
}

string HttpService::pathPrefix() {
  return m_pathPrefix;
}

void HttpService::head(HTTPRequest *request, HTTPResponse *response) {
  cout << "HEAD " << request->getPath() << endl;
  throw ClientError::methodNotAllowed();
}

void HttpService::get(HTTPRequest *request, HTTPResponse *response) {
  cout << "GET " << request->getPath() << endl;
  throw ClientError::methodNotAllowed();
}

void HttpService::put(HTTPRequest *request, HTTPResponse *response) {
  cout << "PUT " << request->getPath() << endl;
  throw ClientError::methodNotAllowed();
}

void HttpService::post(HTTPRequest *request, HTTPResponse *response) {
  cout << "POST " << request->getPath() << endl;
  throw ClientError::methodNotAllowed();
}

void HttpService::del(HTTPRequest *request, HTTPResponse *response) {
  cout << "DELETE " << request->getPath() << endl;
  throw ClientError::methodNotAllowed();
}

