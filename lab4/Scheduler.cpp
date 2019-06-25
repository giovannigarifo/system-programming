//
// Created by giovanni on 24/06/19.
//

#include <iostream>
#include "Scheduler.h"

//constructor
Scheduler::Scheduler() {

    this->ready = priority_queue<Job, vector<Job>, GreaterThanByStartTime>();
    this->running = queue<Job>();
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
        this->workers.push_back(std::move(t));
    }

    cout << "Simulation is starting...number of threads created: " << num_workers << endl;

    // move jobs from ready to running
    while(!ready.empty()){

        Job j = ready.top(); //get top element

        //sleep until job is ready to start
        this_thread::sleep_for(chrono::milliseconds(j.getStart_time()));

        //acquire running mutex
        this->running_mutex.lock();
        this->running.push(j);
        this->running_mutex.unlock();

        ready.pop(); //pop top element
    }
}


// function executed by each worker
void Scheduler::workerExecuteJob() {

    thread::id tid = this_thread::get_id();

    //verify if a job can be executed, if not exit
    running_mutex.lock(); //lock

    while(!running.empty() || !ready.empty()){

        Job j;

        if(running.empty()){
            running_mutex.unlock(); //unlock before sleep
            this_thread::sleep_for(chrono::milliseconds(500));
            running_mutex.lock(); //lock before while
            continue;
        } else {
            j = running.front(); //get job from running queue
            running.pop();
        }

        running_mutex.unlock(); //unlock after getting the job

        //perform the job (simulate it)
        int ms = min(j.getDuration()-j.getExecution_time(), time_quantum);
        this_thread::sleep_for(chrono::milliseconds(ms));

        // update job statistics
        j.setExecution_time(j.getExecution_time() + ms);
        j.setWait_time(0); //TODO

        // push job into right container
        if(j.getExecution_time() == j.getDuration()){

            j.setCompletion_time(0); //TODO

            completed_mutex.lock();
            cout << "Thread " << tid << " fully completed the job " << j.getId() << endl;
            completed.push_back(j);
            completed_mutex.unlock();

        } else {

            running_mutex.lock();
            cout << "Thread " << tid << " worked on the job " << j.getId() << endl;
            running.push(j);
            running_mutex.unlock();
        }

        running_mutex.lock(); //lock before testing while
    }

    running_mutex.unlock(); //unlock before exit
    cout << "Thread " << tid << " is terminating" << endl;
    //terminate();
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
