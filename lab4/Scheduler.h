//
// Created by giovanni on 24/06/19.
//

#ifndef LAB4_SCHEDULER_H
#define LAB4_SCHEDULER_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Job.h"

using namespace std;

/**
 * An implementation of a Round-robin scheduler
 */
class Scheduler {

    priority_queue<Job, vector<Job>, GreaterThanByStartTime> ready; //the smallest one is the first of the queue
    queue<Job> runnable;
    vector<Job> completed;
    vector<thread> workers; //pool of thread that will execute the jobs
    const int time_quantum = 1000;

    mutex mutex_runnable;
    mutex mutex_completed;
    mutex mutex_cv;
    condition_variable cv_running_not_empty;

    void workerExecuteJob();

public:
    Scheduler();
    ~Scheduler();
    void submit(Job job);
    void start();
};


#endif //LAB4_SCHEDULER_H
