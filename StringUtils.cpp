#include "StringUtils.h"

const char* makeMessage(std::string leader, double input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	return buffer.c_str();
}

const char* makeMessage(std::string leader, int input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	return buffer.c_str();
}

const char* makeMessage(std::string leader, char* input)
{
	std::string sinput;
	std::string buffer;
	
	sinput = input;
	buffer = leader + sinput;
	return buffer.c_str();
}
