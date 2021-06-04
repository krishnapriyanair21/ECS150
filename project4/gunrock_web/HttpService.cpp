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
  // check for matching IDs
  if(request->hasAuthToken()){
    // do something
  }
  else{
    fullRequest = request->formEncodedBody();
    username = fullRequest.get("username");
    // ITERATOR DOES NOT WORK
    for(m_db->users<string, User*>::iterator iter = m_db->users.begin(); iter != m_db->users.end(); ++iter){ // loop through database
      if(m_db->users.find(username)){ // if username found
        returnUser->second; // set return pointer to User
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

