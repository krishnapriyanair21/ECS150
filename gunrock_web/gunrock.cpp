#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <deque>

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HttpService.h"
#include "HttpUtils.h"
#include "FileService.h"
#include "MySocket.h"
#include "MyServerSocket.h"
#include "dthread.h"

using namespace std;

int PORT = 8080;
int THREAD_POOL_SIZE = 1;
int BUFFER_SIZE = 1;
string BASEDIR = "static";
string SCHEDALG = "FIFO";
string LOGFILE = "/dev/null";

pthread_mutex_t lockThread = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t roomForThread, requirements = PTHREAD_COND_INITIALIZER;

vector<HttpService *> services;

// define queue structs and create buffer queue
struct node{
  struct node* next;
  MySocket *clientSocket;
};
typedef struct node node_t;
struct queue{
  node_t* head = NULL;
  node_t* tail = NULL;
}
typedef struct queue queue_t;

queue_t<MySocket*> buff[BUFFER_SIZE];

void enqueue(MySocket *clientSocket){
  node_t* nn = new node_t;
  nn->clientSocket = clientSocket;
  if (tail== NULL){
    head = nn;
  } else{
    tail->next = nn;
  }
  tail = nn;
}

MySocket* dequeue(){
  if (head == NULL){
    return NULL;
  } else{
    MySocket* returnVal = head->clientSocket;
    node_t* tempToDelete = head;
    head = head->next;
    if (head == NULL){
      tail = NULL;
    }
    free(tempToDelete);
    return returnVal;
  }
}

HttpService *find_service(HTTPRequest *request) {
   // find a service that is registered for this path prefix
  for (unsigned int idx = 0; idx < services.size(); idx++) {
    if (request->getPath().find(services[idx]->pathPrefix()) == 0) {
      return services[idx];
    }
  }

  return NULL;
}


void invoke_service_method(HttpService *service, HTTPRequest *request, HTTPResponse *response) {
  stringstream payload;
  
  // invoke the service if we found one
  if (service == NULL) {
    // not found status
    response->setStatus(404);
  } else if (request->isHead()) {
    payload << "HEAD " << request->getPath();
    sync_print("invoke_service_method", payload.str());
    cout << payload.str() << endl;
    service->head(request, response);
  } else if (request->isGet()) {
    payload << "GET " << request->getPath();
    sync_print("invoke_service_method", payload.str());
    cout << payload.str() << endl;
    service->get(request, response);
  } else {
    // not implemented status
    response->setStatus(405);
  }
}

void *handle_request(void *pclient) {
  MySocket *client = (MySocket*)pclient;
  free(pclient);
  HTTPRequest *request = new HTTPRequest(client, PORT);
  HTTPResponse *response = new HTTPResponse();
  stringstream payload;
  
  // read in the request
  bool readResult = false;
  try {
    payload << "client: " << (void *) client;
    sync_print("read_request_enter", payload.str());
    readResult = request->readRequest();
    sync_print("read_request_return", payload.str());
  } catch (...) {
    // swallow it
  }    
    
  if (!readResult) {
    // there was a problem reading in the request, bail
    delete response;
    delete request;
    sync_print("read_request_error", payload.str());
    return NULL;
  }
  
  HttpService *service = find_service(request);
  invoke_service_method(service, request, response);

  // send data back to the client and clean up
  payload.str(""); payload.clear();
  payload << " RESPONSE " << response->getStatus() << " client: " << (void *) client;
  sync_print("write_response", payload.str());
  cout << payload.str() << endl;
  client->write(response->response());
    
  delete response;
  delete request;

  payload.str(""); payload.clear();
  payload << " client: " << (void *) client;
  sync_print("close_connection", payload.str());
  client->close();
  delete client;
  return NULL; // MAY NEED TO DELETE
}

// FIFO consumer function
// handle first request in buffer
void *handle_threads(void* arg){
  while(true){
    MySocket* pclient;
    dthread_mutex_lock(&mutex);
    if ((pclient = dequeue()) == NULL){
      dthread_cond_wait(&conditionVar, &mutex);
    }
    dthread_mutex_unlock(&mutex);

    if (pclient!= NULL){
      handle_request(pclient);
    }
  }
}

int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);
  int option;

  while ((option = getopt(argc, argv, "d:p:t:b:s:l:")) != -1) {
    switch (option) {
    case 'd':
      BASEDIR = string(optarg);
      break;
    case 'p':
      PORT = atoi(optarg);
      break;
    case 't':
      THREAD_POOL_SIZE = atoi(optarg);
      break;
    case 'b':
      BUFFER_SIZE = atoi(optarg);
      break;
    case 's':
      SCHEDALG = string(optarg);
      break;
    case 'l':
      LOGFILE = string(optarg);
      break;
    default:
      cerr<< "usage: " << argv[0] << " [-p port] [-t threads] [-b buffers]" << endl;
      exit(1);
    }
  }

  set_log_file(LOGFILE);

  sync_print("init", "");
  MyServerSocket *server = new MyServerSocket(PORT);
  MySocket *client;

  pthread_t newThread[THREAD_POOL_SIZE];
  for (int i = 0; i < THREAD_POOL_SIZE; i++){
    // from while true loop in main
    sync_print("waiting_to_accept", "");
    client = server->accept();
    sync_print("client_accepted", "");
    if (THREAD_POOL_SIZE == 1){
        sync_print("waiting_to_accept", "");
        client = server->accept();
        sync_print("client_accepted", "");
        handle_request(client);
    }
    dthread_create(&newThread[i], NULL, &handle_threads, NULL);
  }
  services.push_back(new FileService(BASEDIR));
  dthread_mutex_lock(&mutex);
  enqueue(client);
  dthread_cond_signal(&conditionVar);
  dthread_mutex_unlock(&mutex);
  // while(true) {
  //   sync_print("waiting_to_accept", "");
  //   client = server->accept();
  //   sync_print("client_accepted", "");
  //   handle_request(client);
  //   pthread_t newThread;
    // MySocket *pclient = (void*)malloc(sizeof(int));
    // *pclient = client;
    
  //   dthread_create(&newThread, NULL, handle_request, pclient);
  // }
}
