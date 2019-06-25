//
// Created by giovanni on 24/06/19.
//

#include "Job.h"


// constructor
Job::Job(int id, int start_time, int duration){

    this->id = id;
    this->start_time = start_time;
    this->duration = duration;

    this->completion_time = 0;
    this->wait_time = 0;
    this->execution_time = 0;
}

int Job::getId() const {
    return id;
}

int Job::getDuration() const {
    return duration;
}

int Job::getExecution_time() const {
    return execution_time;
}

void Job::setExecution_time(int execution_time) {
    Job::execution_time = execution_time;
}

int Job::getWait_time() const {
    return wait_time;
}

void Job::setWait_time(int wait_time) {
    Job::wait_time = wait_time;
}

int Job::getCompletion_time() const {
    return completion_time;
}

void Job::setCompletion_time(int completion_time) {
    Job::completion_time = completion_time;
}


int Job::getStart_time() const {
    return start_time;
}

Job::Job() {}
