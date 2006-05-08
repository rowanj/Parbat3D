#include "iniFile.h"

using namespace std;


void iniFile::open (string fn) {
    if (fn != "") {		// check that a filename has been passed in
        gotFileName = true;	// show that a filename has been set
        fileName = fn;		// store the filename
    } else
        gotFileName = false;	// no filename passed in so show nothing is stored
}


void iniFile::close () {
    gotFileName = false;	// shows that a filename has not been set
    fileName = "";		// store the filename as blank
}


bool iniFile::parse (string key) {
    bool keyInFile = false;			// shows if key has been found in the file
    string line, linekey;			// temp storage for each line input from file
    
    ifstream filePtr (fileName.c_str());	// open the file for reading
    
    if (gotFileName) {
        while (!filePtr.eof()) {		// loop through the entire file
            getline (filePtr, line);		// read in each line of the file
            linekey = getKeyFromLine(line);	// get the key from the line
            
            if (linekey == key) {		// check if the key matches
                keyInFile = true;		// show that the key has been found
                break;
            }
        }
    }
    
    filePtr.close();				// close the file
    
    return keyInFile;				// return whether the key exists in the file
}


string iniFile::read (string key) {
    string data;				// stores the data that corresponds to the key
    string line, linekey;			// temp storage for each line input from file
    
    ifstream filePtr (fileName.c_str());	// open the file for reading
    
    if (gotFileName) {
        while (!filePtr.eof()) {		// loop through the entire file
            getline (filePtr, line);		// read in each line of the file
            linekey = getKeyFromLine(line);	// get the key from the line
            
            if (linekey == key) {		// check if the key matches
                data = getDataFromLine(line);	// store the data from the key
                break;
            }
        }
    }
    
    filePtr.close();				// close the file
    
    return data;				// return the data value
}


void iniFile::update (string key, string data) {
    string line, linekey;	// temp storage for each line input from file
    
    if (gotFileName) {
        if (parse(key)) {	// check if key is in file
            string updated;	// stores the entire file temporarily before writing
            string temp;	// temp storage for the new data for the file
            
            
            // read in contents of file, updating the data when necessary
            ifstream filePtrI (fileName.c_str());	// open the file for reading
            
            while (!filePtrI.eof()) {			// loop through the entire file
                getline (filePtrI, line);		// read in each line of the file
                linekey = getKeyFromLine(line);		// get the key from the line
                
                if (linekey == key)			// check if the key matches
                    temp = key + '=' + data + '\n';	// update the key's data
                else
                    temp = line + '\n';			// use original data
                
                if (temp != "\n")			// make sure last line is not duplicated
                    updated += temp;			// add the line to the contents
            }
            
            filePtrI.close();				// close the file
            
            
            // write contents back to file
            ofstream filePtrO (fileName.c_str(), ios::trunc);	// open the file, erasing all of its contents
            filePtrO << updated;				// add the updated contents to the file
            filePtrO.close();					// close the file
        } else {
            ofstream filePtr (fileName.c_str(), ios::app);	// open the file, positioning the write pointer at the end
            filePtr << key << '=' << data << '\n';		// add the new data to the file
            filePtr.close();					// close the file
        }
    }
}


string iniFile::getKeyFromLine (string line) {
    int epos;				// position of '=' in the line
    string key = "";			// key text from the line
    
    epos = line.find("=", 0);		// find where '=' is in the line
    
    if (epos>0)				// if '=' does exist in the line then all text before it must be the key
        key = line.substr(0,epos);	// get the key from the line
    
    return key;				// return the key's value
}


string iniFile::getDataFromLine (string line) {
    int epos;				// position of '=' in the line
    string data = "";			// data text from the line
    
    epos = line.find("=", 0);		// find where '=' is in the line
    
    if (epos>0)				// if '=' does exist in the line then all text after it must be the data
        data = line.substr(epos+1);	// get the data from the line
    
    return data;			// return the data value
}
