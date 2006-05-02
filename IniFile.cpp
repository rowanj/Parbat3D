#include <iostream>

#include "iniFile.h"

using namespace std;


void iniFile::open (string fn) {
    filePtr.open(fn.c_str());
    
    if (filePtr.is_open()) {
        fileIsOpen = true;
        fileName = fn;
    } else
        fileIsOpen = false;
}


void iniFile::close (string fn) {
    if (fileName==fn) {
        filePtr.close();
        fileName = "";
    }
}


bool iniFile::parse (string key) {
    bool keyInFile = false;
    string line, linekey;
    int epos;
    
    if (fileIsOpen) {
        filePtr.seekg(0, ios::beg); // doesn't work
        
        while (!filePtr.eof()) {
            getline (filePtr, line);
            epos = line.find("=", 0);
            if (epos>0) {
                linekey = line.substr(0,epos);
                if (linekey==key) {
                    keyInFile = true;
                    break;
                }
            }
        }
    }
    
    return keyInFile;
}


string iniFile::read (string key) {
    string data;
    string line, linekey;
    int epos;
    
    if (fileIsOpen) {
        filePtr.seekg(0, ios::beg); // doesn't work
        
        while (!filePtr.eof()) {
            getline (filePtr, line);
            epos = line.find("=", 0);
            if (epos>0) {
                linekey = line.substr(0,epos);
                if (linekey==key) {
                    data = line.substr(epos+1);
                    break;
                }
            }
        }
    }
    
    return data;
}


void iniFile::update (string key, string data) {
    string line, linekey;
    string temp;
    int epos;
    
    string tempfilename = '~' + getFileName();
    fstream tempfile (tempfilename.c_str(), ios::out | ios::app);
    
    if (fileIsOpen) {
        if (parse(key)) {  // check if key fileIsOpen in file
            while (!filePtr.eof()) {
                getline (filePtr, line);
                epos = line.find("=", 0);
                if (epos>0) {
                    linekey = line.substr(0,epos);
                    
                    if (linekey==key)
                        tempfile << key << '=' << data << '\n';
                    else
                        tempfile << line;
                }
            }
            
            // replace file contents with tempfile contents
        } else {
            filePtr.seekp(0, ios::end);
            filePtr << key << '=' << data << '\n';
        }
    }
}
