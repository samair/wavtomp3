#include <iostream>
#include <string>
#include <queue>
#include <stdio.h>
#ifdef _WIN32
  #include <windows.h>

#else
  #include <unistd.h>
#endif

#include <pthread.h>
#include <stdlib.h>
using namespace std;

bool DEBUG = false;

class Task{
public:
  virtual void run()=0;
};


class TaskQ{

private:

    std::queue<Task*> vTask;
    pthread_mutex_t task_mutex;
    pthread_cond_t  cond_push;
public:
    bool shouldStop;
  TaskQ(){
    shouldStop=false;
     pthread_mutex_init(&task_mutex,0);
      pthread_cond_init(&cond_push, 0);
  }
  void pushTask(Task* task)
  {
    if (!shouldStop) {
      pthread_mutex_lock(&task_mutex);
      vTask.push(task);
      pthread_cond_signal(&cond_push);
      pthread_mutex_unlock(&task_mutex);
    }
  }
  Task* getTask() {
    if (DEBUG)
    cout<<"trying to get task , is stop ?"<< shouldStop<< "Q size: "<<vTask.size() <<endl;
    pthread_mutex_lock(&task_mutex);

    if ((shouldStop == true) && (vTask.size() == 0))
    {
      if (DEBUG)
      cout<<"lets exit!!"<<endl;
      pthread_mutex_unlock(&task_mutex);
      return NULL;
    }
    else{
      Task* task = NULL;

      if (vTask.size() ==0){
        if (DEBUG)
        cout<<"On wait"<<endl;
        pthread_cond_wait(&cond_push, &task_mutex);
      }
       //pop only if queue has something
       if (vTask.size() !=0 ){
        task = vTask.front();
        vTask.pop();
      }


        pthread_mutex_unlock(&task_mutex);
        return task;
      }
    }


  void wait()
  {
    while(vTask.size() !=0){
      sleep(1);
    }
  }
  void finish(int numThreads)
  {
    shouldStop=true;
    pthread_mutex_unlock(&task_mutex);
    if (DEBUG)
    cout<<"took lock "<<endl;
    int rc = pthread_cond_broadcast(&cond_push);
    if (DEBUG)
    cout<<"signaled : "<<rc<<endl;
  //  pthread_mutex_unlock(&task_mutex);

  }
};

void* assignTask(void * param){
  // get the tasks from task q
  if (DEBUG)
  cout<<"Assign task"<<endl;
  TaskQ* Q = (TaskQ*)param;
  Task* t = NULL;
  while((t = Q->getTask()) && t !=NULL){
    if (DEBUG)
    cout<<"Got task"<<endl;
    if (t == NULL)
     break;
    t->run();
  }
  if (DEBUG)
  cout<<"Done"<<endl;

}

class ThreadPool{

private:
  unsigned int numThr;
  pthread_t* workerThreads;
  TaskQ* taskQ;

public:

  ThreadPool(unsigned int numThreads):numThr(numThreads)
  {
    //ceate a array of threads
    workerThreads = new pthread_t[numThr];

    //create a task Q
    taskQ = new TaskQ();
    // assign a global function
    for (int i=0; i< numThr; ++i) {
    //  cout<<"DEBUG: "<<"Creating thread"<<endl;
      pthread_create(&(workerThreads[i]),0,assignTask,taskQ);
    }
  }
  ~ThreadPool()
  {
     wait();
    for (int i=0; i< numThr; ++i){
      pthread_join(workerThreads[i],0);
    }
  }
  void addTask(Task* t)
  {
    taskQ->pushTask(t);
  }
  void wait(){
    taskQ->wait();
  }
  void stop(){
    //check if there are
    taskQ->finish(numThr);
  }
};
