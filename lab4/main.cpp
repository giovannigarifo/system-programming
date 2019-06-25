#include <iostream>
#include "Scheduler.h"
#include "Job.h"

int main() {

    Scheduler* p = new Scheduler();

    p->submit(Job(1,0,1500)); // Job(int id, int start_time, int duration)
    p->submit(Job(2,0,600));
    p->submit(Job(3,100,900));
    p->submit(Job(4,200,1200));
    p->submit(Job(5,300,1600));
    p->submit(Job(6,300,500));
    p->submit(Job(7,400,700));
    p->submit(Job(8,400,600));
    p->submit(Job(9,500,900));

    p->start();

    delete p; //calls the destructor of p
}