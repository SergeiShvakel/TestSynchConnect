#ifndef SERVERASYNCH_H_
#define SERVERASYNCH_H_

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "ThreadPoolManager/threadmanager.h"

namespace asio = boost::asio;

typedef
boost::shared_ptr<asio::ip::tcp::socket> socket_ptr;

class ServerAsynch
{
public:
	ServerAsynch(ThreadManager *pthrmanager);
	~ServerAsynch();

	int start();
	void shutdown();

private:
	ThreadManager *m_thrmanager;			// manager threads
	asio::ip::tcp::acceptor *m_acceptor;	// acceptor
	asio::ip::tcp::endpoint  m_endpoint;

	const std::string messServerThread;		// Debug Message

	void start_accept();
	void handle_accept (socket_ptr sock,
						const boost::system::error_code& err);

	void job_accept(int threadID,
					socket_ptr sock,
					const boost::system::error_code& err);


};

#endif SERVERASYNCH_H_