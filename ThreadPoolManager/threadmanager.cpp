#include <cstdlib>
#include <algorithm>

#include "ThreadPoolManager/threadmanager.h"

ThreadManager::ThreadManager(size_t threadsCount)
{
    if (threadsCount == 0)
        threadsCount = 1;

    for (size_t i=0; i<threadsCount; i++)
    {
        PThreadJob pJob = new ThreadJob(i+1);
        m_threadjob.push_back(pJob);
    }
}

ThreadManager::~ThreadManager()
{
    std::for_each(m_threadjob.begin(), m_threadjob.end(), [](PThreadJob job){
        delete job;
        }
    );
}

PThreadJob ThreadManager::getFreeJob()
{
	PThreadJob pthreadjob = nullptr;
	size_t minTasks = UINT32_MAX;

    int num = (rand ()%m_threadjob.size()); // random number from 0 to m_threadjob.size()-1

    pthreadjob = m_threadjob[num];

	/*std::for_each (m_threadjob.begin(), m_threadjob.end(), [&](PThreadJob it)
    {
        if (it->isEmpty())
		{
			pthreadjob = it;
            return;            
		}
		else if (minTasks > it->getJobsCount())
		{
			minTasks = it->getJobsCount();
			pthreadjob = it;
		}
    });*/
	
	return pthreadjob;
}

void ThreadManager::runService()
{
    std::for_each(m_threadjob.begin(), m_threadjob.end(), [&](PThreadJob it){
        fn_type job = [&](){ m_service.run(); };
        it->addJob(job);
    });
}

void ThreadManager::stopService()
{
    m_service.stop();
    
    std::for_each(m_threadjob.begin(), m_threadjob.end(), [&](PThreadJob it){
        it->stopJob();
    });
}

int ThreadManager::getCurrentThreadID(const std::thread::id& _id)
{
    int threadID = 0;
    std::for_each(m_threadjob.begin(), m_threadjob.end(), [&](PThreadJob it){
        if (*it == _id)
        {
            threadID = it->getThreadID();
            return;
        }
    });

    return threadID;
}

asio::io_service& ThreadManager::getm_Service()
{
    return m_service;
}

void ThreadManager::join_all()
{
    std::for_each(m_threadjob.begin(), m_threadjob.end(), [&](PThreadJob it){
        it->join();
    });
}