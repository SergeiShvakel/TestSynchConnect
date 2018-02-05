#include <iostream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "ServerAsynch/ServerAsynch.h"
#include "ClientRequest/ClientRequest.h"

#include "ThreadPoolManager/threadmanager.h"

int main(int argc, char* argv[])
{
	std::cout << "Start programm...\r\n";
	std::cout << "Hardware max thread count to run: " << boost::thread::hardware_concurrency() << std::endl;
	std::cout << "To ShutdownServer press 'Q' and 'Enter'" << std::endl;

	ThreadManager threadManager(5);
	ServerAsynch serverEcho(&threadManager);

	// luanch server thread
	std::thread thread_server (std::bind(ServerAsynch::start, &serverEcho));

	// luanch client's threads
	std::vector<std::thread*> threads;
	for (int i = 0; i < 10; i++)
	{
		std::thread *client_thr = new std::thread(std::bind(send_request, i+1));
		threads.push_back(client_thr);
	}
	std::for_each (threads.begin(), threads.end(), [](std::thread* t){
		t->join();
	});
	
	char q = 0;
	std::cin >> q;
	if (q == 'q' || q == 'Q'){
		serverEcho.shutdown();
	}

	thread_server.join();
	
	std::cout << "Press any key and Return for Exit...";
	std::cin >> q;

	std::for_each (threads.begin(), threads.end(), [](std::thread* t){
		delete t;
	});

  	return 0;
}