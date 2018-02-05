#ifndef THREADMANAGER_H_
#define THREADMANAGER_H_

#include <boost/asio.hpp>
#include "ThreadPoolManager/threadjob.h"

namespace asio = boost::asio;

class ThreadManager
{
public:

	ThreadManager(size_t threadsCount = 1);
	~ThreadManager();

    PThreadJob getFreeJob();

	int  getCurrentThreadID(const std::thread::id& _id);

	asio::io_service& getm_Service();

	void runService();
	void stopService();

	void join_all();

private:
	std::vector<PThreadJob> m_threadjob; // array of threads
	asio::io_service m_service;
};

#endif // #define THREADMANAGER_H_