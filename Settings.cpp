#include "Settings.h"

using namespace std;


void settings::setSetting (string key, string data) {
    openFile.update(key, data);
}


string settings::getSetting (string key) {
    string data;
    
    data = openFile.read(key);
    
    return data;
}
