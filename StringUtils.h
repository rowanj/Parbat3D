#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <iostream>

/*This simple file contains utils to perform some tricky operations between
C++ strings and C strings (and ints) that were giving me the heebies for
about 4 hours today.*/

/*The makeMessage function takes a C++ string and either an int or a C string
and concatenates them, and returns a const char* suitable for a MessageBox.
Actually catting a string and a char* together is a bothersome process,
and trying to cat a char* and a char* together proved impossible... well,
for MessageBox to understand, anyway. Crashes abounded.

The int overloaded function is my solution for forcing an int into a char*
readable format. This was not easy, and I'm using the stringstream class
to do it. Other solutions were offered online, but all involved mod, and
my method was easier for me to code. I make no guarantees of efficiency.

Be warned, none of these allocate memory, and they almost certainly should.
Yet they work. This may mean they could blow up if you try to put too much
data into them. You have been warned!*/

const char* makeMessage(std::string leader, int input);
const char* makeMessage(std::string leader, double input);
const char* makeMessage(std::string leader, char* input);

#endif
