#include <iostream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "ServerAsynch/ServerAsynch.h"
#include "TraceFunction/TraceFunction.h"

size_t read_complete(char* buff, const boost::system::error_code & err, size_t bytes) 
{
	if (err) return 0;

	auto find_symb = std::find(buff, buff + bytes, '\n');
	bool found = find_symb < (buff+bytes)/*last element*/;

	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}

ServerAsynch::ServerAsynch(ThreadManager *pthrmanager)
	: messServerThread("server thread:")
{
	m_thrmanager = pthrmanager;
}

ServerAsynch::~ServerAsynch()
{
	delete m_acceptor;
}

int ServerAsynch::start() 
{
    // debug
    trace_executing(messServerThread, "start");

	boost::system::error_code ec;
	m_endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"),8001);

	m_acceptor = new asio::ip::tcp::acceptor(m_thrmanager->getm_Service());
	m_acceptor->open(m_endpoint.protocol());
	//m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
    m_acceptor->bind(m_endpoint, ec);
	if (ec != 0)
	{
		std::cout << ec << std::endl;
		return 0;
	}
	m_acceptor->listen();
	
	// asynchronous call of accept
    start_accept();

	m_thrmanager->runService();
	m_thrmanager->join_all();
	
	return 0;
}

void ServerAsynch::shutdown()
{
	boost::system::error_code ec;

	m_thrmanager->stopService();
	m_acceptor->close(ec);
}

void ServerAsynch::start_accept()
{
	// debug
	trace_executing(messServerThread, "accepting connection...");

	socket_ptr sock(new asio::ip::tcp::socket(m_thrmanager->getm_Service()));
	m_acceptor->async_accept (*sock, boost::bind(ServerAsynch::handle_accept, this, sock, _1));
}

void ServerAsynch::handle_accept (socket_ptr sock,
								  const boost::system::error_code& err)
{
	// handle error
	if (err) // The I/O operation has been aborted because of either a thread exit or an application reques
	{
		// close socket
		sock->close();
		return;
	}

	// New asynchronous call of accept
    start_accept();

	int nThreadID = m_thrmanager->getCurrentThreadID(std::this_thread::get_id());
	job_accept(nThreadID, sock, err);
}

void ServerAsynch::job_accept(int threadID,
							  socket_ptr sock,
							  const boost::system::error_code& err)
{
	std::string messServerThread;

	messServerThread.append(2, '\t');
	char temp[64] = "";
	snprintf(temp, sizeof(temp), "job thread#%d",threadID);
	messServerThread.append(temp);

	// debug
	trace_executing(messServerThread, "start handle accept");

	// debug
	trace_executing(messServerThread, "start reading text...");

	char buff[1024] = "";
	boost::system::error_code error;

	int bytes = asio::read(*sock, asio::buffer(buff), boost::bind(read_complete,buff,_1,_2), error);

	std::string msg(buff, bytes);

	// debug
	char buff0[256] = "";
  	snprintf(buff0, sizeof(buff0), "%s%s", "recieved text: ", msg.c_str());
	trace_executing(messServerThread, buff0);

	// change text
	auto findpos = msg.find("server");
	if (findpos != std::string::npos)
	{
		auto findposclient = msg.find ("client N");
		std::string strclient_n = "client";

		if (findposclient != std::string::npos)
		{
			strclient_n = msg.substr(findposclient, std::string::npos);

			findposclient = strclient_n.rfind (" ");
			if (findposclient != std::string::npos)
			{
				strclient_n = strclient_n.substr(0, findposclient);
			}
		}

		msg.replace(findpos, 6, strclient_n.c_str());

		findpos = msg.rfind(strclient_n.c_str());
		if (findpos != std::string::npos)
			msg.replace(findpos, strclient_n.length(), "server");
	}

	// debug
	snprintf(buff0, sizeof(buff0), "%s%s", "send response: ", msg.c_str());
	trace_executing(messServerThread, buff0);

	// send text back to client
	sock->write_some(asio::buffer(msg));

	// close socket
	sock->close();
	
	// debug
	trace_executing(messServerThread, "exit handle accept");
}
