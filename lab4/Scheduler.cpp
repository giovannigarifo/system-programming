//
// Created by giovanni on 24/06/19.
//

#include <iostream>
#include "Scheduler.h"

//constructor
Scheduler::Scheduler() {

    this->ready = priority_queue<Job, vector<Job>, GreaterThanByStartTime>();
    this->runnable = queue<Job>();
    this->completed = vector<Job>();
    this->workers = vector<thread>();
}


// add a job to the ready queue
void Scheduler::submit(Job job) {

    // thread safe because is done before threads are created
    this->ready.push(job);
}


//start the execution
void Scheduler::start() {

    // create workers
    int num_workers = thread::hardware_concurrency();
    if(num_workers == 0)
        num_workers = 2;

    for(int i=0; i< num_workers; i++){

        thread t = thread(&Scheduler::workerExecuteJob, this);
        workers.push_back(std::move(t));
    }

    cout << "Simulation is starting...number of threads created: " << num_workers << endl;

    // move jobs from ready to runnable
    while(!ready.empty()){

        Job j = ready.top(); //get top element

        //sleep until job is ready to start
        this_thread::sleep_for(chrono::milliseconds(j.getStart_time()));

        //acquire runnable mutex
        lock_guard<mutex> lg_r(mutex_runnable);
        lock_guard<mutex> lg_cv(mutex_cv);
        runnable.push(j);
        cv_running_not_empty.notify_one();

        ready.pop(); //pop top element
    }
}


// function executed by each worker
void Scheduler::workerExecuteJob() {

    thread::id tid = this_thread::get_id();

    //verify if a job can be executed, if not exit
    mutex_runnable.lock(); //lock

    while(!runnable.empty() || !ready.empty()){ //until there are jobs still to be finished

        Job j;

        //wait for job to be available
        while(runnable.empty()){
            mutex_runnable.unlock();
            unique_lock<mutex> ul(mutex_cv);
            cv_running_not_empty.wait_for(ul, chrono::duration<int>(1), [this]()->bool{return !runnable.empty();}); //wait and release lock
            mutex_runnable.lock();

            if(runnable.empty() && ready.empty()){ //woke up but nothing to do left
                mutex_runnable.unlock();
                return;
            }
        }

        //job available, get the job
        j = runnable.front();
        runnable.pop();
        mutex_runnable.unlock();

        //perform the job (simulate it)
        int ms = min(j.getDuration()-j.getExecution_time(), time_quantum);
        this_thread::sleep_for(chrono::milliseconds(ms));

        // update job statistics
        j.setExecution_time(j.getExecution_time() + ms);
        j.setWait_time(0); //TODO

        // push job into right container
        if(j.getExecution_time() == j.getDuration()){

            j.setCompletion_time(0); //TODO

            lock_guard<mutex> lg(mutex_completed);
            cout << "Thread " << tid << " fully completed the job " << j.getId() << endl;
            completed.push_back(j);

        } else {

            lock_guard<mutex> lg(this->mutex_runnable);
            cout << "Thread " << tid << " worked on the job " << j.getId() << endl;
            runnable.push(j);
            cv_running_not_empty.notify_one(); //notify that a job can be run
        }

        mutex_runnable.lock(); //lock before testing while
    }

    mutex_runnable.unlock(); //unlock before exit
    cout << "Thread " << tid << " is terminating" << endl;
}


Scheduler::~Scheduler() {

    for(thread& worker : this->workers){
        worker.join();
    }

    cout << "\n\nScheduler reporting: \n" <<endl;
    for( Job j : this->completed){
        cout << "Job "<< j.getId() << " has been finished." << endl;
    }
    cout << "\n...Scheduler say bye!" <<endl;
}
