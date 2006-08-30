#include "Settings.h"

using namespace std;


settings::settings (string s) {
    open(s);
}

settings::~settings () {
    openFile.close();
}


void settings::open (string filename) {
    openFile.open(filename);
}

string settings::getSetting(string section, string key)
{
	return getSetting(section, key, "");
}

string settings::getSetting (string section, string key, string defaultSetting)
{
    string data = defaultSetting;
    
    try {
        if (openFile.parse(section, key))
            data = openFile.read(section, key);
    } catch (...) {setSetting(section, key, defaultSetting);}
    
    return data;
}

int settings::getSettingi(string section, string key)
{
	return getSettingi(section, key, 0);
}


int settings::getSettingi (string section, string key, int defaultSetting)
{
	string d;
    stringstream ss;
    
    ss << defaultSetting;
    ss >> d;
	string data = d;
	
	int value = 0;
	try {
        if (openFile.parse(section, key))
            data = openFile.read(section, key);
    } catch (...) {setSetting(section, key, defaultSetting);}
    value = atoi(data.c_str());
	return value;
}


void settings::setSetting (string section, string key, string data) {
    openFile.update(section, key, data);
}


void settings::setSetting (string section, string key, int data) {
    string d;
    stringstream ss;
    
    ss << data;
    ss >> d;
    
    openFile.update(section, key, d);
}
