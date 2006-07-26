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

std::string makeString(std::string leader, double input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	return buffer;
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
const char* makeMessage(std::string leader, unsigned int input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	return buffer.c_str();
}

std::string makeString(std::string leader, int input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	return buffer;
}

const char* makeMessage(std::string leader, char* input)
{
	std::string sinput;
	std::string buffer;
	
	sinput = input;
	buffer = leader + sinput;
	return buffer.c_str();
}

std::string makeString(std::string leader, char* input)
{
	std::string sinput;
	std::string buffer;
	
	sinput = input;
	buffer = leader + sinput;
	return buffer;
}

const char* catcstrings(char* input1, char* input2)
{
	std::string sinput1, sinput2;
	std::string buffer;
	
	sinput1 = input1;
	sinput2 = input2;
	buffer = sinput1 + sinput2;
	return buffer.c_str();
}

const char* inttocstring(int input)
{
    std::string sinput;
	std::stringstream s;
	
	s << input;
	s >> sinput;
	return sinput.c_str();
}


char* copyString(char *oldstr)
{
    char *newstr;
    int str_length=strlen(oldstr);
    newstr=new char[str_length+1];
    strcpy(oldstr,newstr);
    return newstr;
}
