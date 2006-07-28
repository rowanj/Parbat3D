#include "Settings.h"

using namespace std;


string settings::getSetting (string section, string key) {
    string data = "";
    
    try {
        if (openFile.parse(section, key))
            data = openFile.read(section, key);
    } catch (...) {}
    
    return data;
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

