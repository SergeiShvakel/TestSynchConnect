#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/thread.hpp>

#include "TraceFunction/tracefunction.h"

namespace asio = boost::asio;

void send_request(int idRequest)
{
	// debug
  	std::string messClientThread;

	char buff0[1024] = "";
	snprintf(buff0, sizeof(buff0), "\t%s%d: ", "client thread", idRequest);
	messClientThread = buff0;

	// debug
	trace_executing (messClientThread, "start");

	asio::io_service io_service;
  	asio::ip::tcp::socket socket(io_service);

	// 178.172.160.5
	boost::system::error_code error;
	
	bool bConnected = false;
	int i = 0;
	do{
		socket.connect( asio::ip::tcp::endpoint( asio::ip::address::from_string("127.0.0.1"), 8001 ), error );
		if( error )
		{
			// debug
			trace_executing (messClientThread, "connect failed to 127.0.0.1 ...waiting for 1000 msc ");

			boost::this_thread::sleep( boost::posix_time::millisec(1000));
		}
		else
		{
			// debug
			trace_executing (messClientThread, "connected successful");

			bConnected = true;
		}

		i++;

	} while (!bConnected && i < 10);
  	
	if (!bConnected)
	{
		// debug
		trace_executing (messClientThread, "unable to connect server");

		return;
	}

	// debug
	trace_executing (messClientThread, "send data...");

	snprintf(buff0, sizeof(buff0), "Hello, server. I'm client N%d \n", idRequest);
 	const std::string msg = buff0;

  	asio::write( socket, asio::buffer(msg), error );
  	if( error )
	{
		// debug
		snprintf(buff0, sizeof(buff0), "%s%s: ", "send failed: ", error.message());
		trace_executing (messClientThread, buff0);
  	}
  	else {
		// debug
		trace_executing (messClientThread, "send correct!");
  	}

	// debug
	trace_executing (messClientThread, "read response from server");

	asio::streambuf receive_buffer;
  	asio::read( socket, receive_buffer, asio::transfer_all(), error );

  	if( error && error != asio::error::eof )
	{
		// debug
		snprintf(buff0, sizeof(buff0), "%s%s ", "receive failed: ", error.message());
		trace_executing (messClientThread, buff0);
 	}
  	else {
    	const char* data = asio::buffer_cast<const char*>(receive_buffer.data());

		// debug
		snprintf(buff0, sizeof(buff0), "%s%s ", "received: ", data);
		trace_executing (messClientThread, buff0);
  	}

	socket.close();

	// debug
	trace_executing (messClientThread, "socket closed");
}