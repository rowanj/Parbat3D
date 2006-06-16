#include "Settings.h"

using namespace std;


string settings::getSetting (string key) {
    string data = "";
    
    try {
        if (openFile.parse(key))
            data = openFile.read(key);
    } catch (...) {}
    
    return data;
}


void settings::setSetting (string key, string data) {
    openFile.update(key, data);
}


void settings::setSetting (string key, int data) {
    string d;
    stringstream ss;
    
    ss << data;
    ss >> d;
    
    openFile.update(key, d);
}

