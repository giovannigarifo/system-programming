//
// Created by giovanni on 24/06/19.
//

#ifndef LAB4_JOB_H
#define LAB4_JOB_H


class Job {

private:
    int id;
    int duration; // how much time the job requires
    int execution_time; // how much time has already been dedicated to the job
    int start_time; // when the job can start
    int wait_time; //how much time the job has waited, even if ready
    int completion_time; //when the job has been completed
public:
    int getStart_time() const;

public:
    Job(int id, int start_time, int duration);

    Job();

    int getExecution_time() const;

    void setExecution_time(int execution_time);

    int getWait_time() const;

    void setWait_time(int wait_time);

    int getCompletion_time() const;

    void setCompletion_time(int completion_time);

    int getId() const;

    int getDuration() const;

};


struct GreaterThanByStartTime
{
    bool operator()(const Job& j1, const Job& j2) const
    {
        return j1.getStart_time() > j2.getStart_time();
    }
};

#endif //LAB4_JOB_H
