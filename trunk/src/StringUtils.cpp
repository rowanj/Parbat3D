#include "PchApp.h"

#include "StringUtils.h"


const char* makeMessage(std::string leader, double input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	char* final;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	
	final = new char[buffer.length()+1];
    strcpy(final, buffer.c_str());
	return (const char*) final;
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
	char* final;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	
	final = new char[buffer.length()+1];
    strcpy(final, buffer.c_str());
	return (const char*) final;
}
const char* makeMessage(std::string leader, unsigned int input)
{
	std::string sinput;
	std::string buffer;
	std::stringstream s;
	char* final;
	
	s << input;
	s >> sinput;
	buffer = leader + sinput;
	
	final = new char[buffer.length()+1];
    strcpy(final, buffer.c_str());
	return (const char*) final;
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
	char* final;
	
	sinput = input;
	buffer = leader + sinput;
	
	final = new char[buffer.length()+1];
    strcpy(final, buffer.c_str());
	return (const char*) final;
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
	char* final;
	
	sinput1 = input1;
	sinput2 = input2;
	buffer = sinput1 + sinput2;
	
	final = new char[buffer.length()+1];
    strcpy(final, buffer.c_str());
	return (const char*) final;
}

const char* inttocstring(int input)
{
    std::string sinput;
	std::stringstream s;
	char* final;
	
	s << input;
	s >> sinput;
	
	final = new char[sinput.length()+1];
    strcpy(final, sinput.c_str());
	return (const char*) final;
}

const char* floattocstring(float input)
{
    std::string sinput;
	std::stringstream s;
	char* final;
	
	fixed(s);
	s.precision(4);
	s << input;
	s >> sinput;
	
	final = new char[sinput.length()+1];
    strcpy(final, sinput.c_str());
	return (const char*) final;
}

char* copyString(const char *oldstr)
{
    char *newstr;
    int str_length=strlen(oldstr);
    newstr=new char[str_length+1];
    strcpy(newstr,(char*)oldstr);
    return newstr;
}


int stringToInt (std::string str) {
    int v;
    std::stringstream ss;
    ss << str;
    ss >> v;
    return v;
}

int stringToInt (char* str) {
    int v;
    std::stringstream ss;
    ss << str;
    ss >> v;
    return v;
}
