#ifndef THREADJOB_H_
#define THREADJOB_H_

#include <condition_variable>
#include <mutex>
#include <functional>
#include <thread>
#include <queue>
#include <memory>

typedef
std::function<void()> fn_type;

class ThreadJob
{
public:
    ThreadJob (int nID);
    ~ThreadJob ();

    void addJob (fn_type newJob);
    void stopJob();
    int  getJobsCount();
    bool isEmpty();
    int  getThreadID();
    
    void join();

    bool operator == (const std::thread::id& _id);

private:
    int                         m_nID;      // inner ID job
    bool                        m_enabled;  // determine if thread is working
    std::condition_variable     m_status;   // member for sleep wait
    std::queue<fn_type>         m_jobqueue; // queue for jobs to execute
    std::mutex                  m_mutex;    // object for synchronization 

    std::thread                 m_thread;   // thread

	void thread_func();                     // main thread func with loop
};

typedef
ThreadJob* PThreadJob;

#endif // #define THREADJOB_H_