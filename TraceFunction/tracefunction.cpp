#include <iostream>

void trace_executing (const std::string& prefix_str, const char* str)
{
	char  buff0[1024] = "";
	snprintf(buff0, sizeof(buff0), "%s: %s\r\n", prefix_str.c_str(), str);
	std::cout << buff0;
}