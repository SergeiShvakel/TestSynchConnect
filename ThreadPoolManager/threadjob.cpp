#include "threadjob.h"

ThreadJob::ThreadJob (int nID)
{
	m_nID = nID;
    m_enabled = true;

    m_thread = std::thread(ThreadJob::thread_func, this);
}

ThreadJob::~ThreadJob ()
{
    m_enabled = false;    	// should stop thread
    m_status.notify_one();  // 
}

void ThreadJob::addJob (fn_type newJob)
{
    std::unique_lock<std::mutex> locker(m_mutex);

    m_jobqueue.push (newJob);

    m_status.notify_one(); // notify object to check status
}

void ThreadJob::stopJob()
{
	m_enabled = false;    	// should stop thread
    m_status.notify_one();  // 
}

int ThreadJob::getJobsCount()
{
	m_jobqueue.size();
}

bool ThreadJob::isEmpty()
{
	return m_jobqueue.empty();
}

int  ThreadJob::getThreadID()
{
	return m_nID;
}

void ThreadJob::thread_func()
{
    while (m_enabled)
	{
		std::unique_lock<std::mutex> locker(m_mutex);

		m_status.wait(locker, [&](){ return !m_jobqueue.empty() || !m_enabled; });

		while (!m_jobqueue.empty())
		{
            // get new job to run
			fn_type fn = m_jobqueue.front();
			locker.unlock();

			fn();

			// exctract job from queue
			locker.lock();
			m_jobqueue.pop();
		}				
	}
}

void ThreadJob::join()
{
	m_thread.join();
}

bool ThreadJob::operator == (const std::thread::id& _id)
{
	return (m_thread.get_id() == _id);
}