#include "IniFile.h"

using namespace std;


void iniFile::open (string fn) {
    if (fn != "") {                                                    // check that a filename has been passed in
        gotFileName = true;                                            // show that a filename has been set
        fileName = fn;                                                 // store the filename
    } else
        gotFileName = false;                                           // no filename passed in so show nothing is stored
}


void iniFile::close () {
    gotFileName = false;                                               // shows that a filename has not been set
    fileName = "";                                                     // store the filename as blank
}


bool iniFile::parse (string section, string key) {
    bool keyInFile = false;                                            // shows if key has been found in the file
    string line, linekey;                                              // temp storage for each line input from file
    string sectionLabel ('[' + section + ']');                         // section text as it appears in the file
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());                           // open the file for reading
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {                                   // find the appropriate section
                getline (filePtr, line);
                if (line == sectionLabel) break;
            }
            
            while (!filePtr.eof()) {                                   // loop through rest of file
                getline (filePtr, line);                               // read in each line of the file
                
                if (line[0] == '[') break;                             // check for end of section
                
                linekey = getKeyFromLine(line);                        // get the key from the line
                
                if (linekey == key) {                                  // check if the key matches
                    keyInFile = true;                                  // show that the key has been found
                    break;
                }
            }
            
            filePtr.close();                                           // close the file
        }
    }
    
    return keyInFile;                                                  // return whether the key exists in the file
}


string iniFile::read (string section, string key) {
    string data = "";                                                  // stores the data that corresponds to the key
    string line, linekey;                                              // temp storage for each line input from file
    string sectionLabel ('[' + section + ']');                         // section text as it appears in the file
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());                           // open the file for reading
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {                                   // find the appropriate section
                getline (filePtr, line);
                if (line == sectionLabel) break;
            }
            
            while (!filePtr.eof()) {                                   // loop through rest of file
                getline (filePtr, line);                               // read in each line of the file
                
                if (line[0] == '[') break;                             // check for end of section
                
                linekey = getKeyFromLine(line);                        // get the key from the line
                
                if (linekey == key) {                                  // check if the key matches
                    data = getDataFromLine(line);                      // store the data from the key
                    break;
                }
            }

            filePtr.close();                                           // close the file
        }
    }
    
    return data;                                                       // return the data value
}


void iniFile::update (string section, string key, string data) {
    string line, linekey;                                              // temp storage for each line input from file
    bool newLinePending = false;                                       // used to add whitespace but stop last line duplication
    string sectionLabel ('[' + section + ']');                         // section text as it appears in the file
    bool inSection = false;
    bool dataAdded = false;
    
    if (gotFileName) {
        // check that the section exists
        if (sectionStartsAt(section) > 0) {
            ifstream filePtrI (fileName.c_str());                      // open the file for reading
            
            if (filePtrI.is_open()) {
                string updated = "";                                   // stores the entire file temporarily before writing
                string temp;                                           // temp storage for the new data for the file
                
                // read in contents of file, updating the data when necessary
		        while (!filePtrI.eof()) {                              // loop through the entire file
			        if (newLinePending) {
                        updated += '\n';
                        newLinePending = false;
                    }
                    
			        getline (filePtrI, line);                          // read in each line of the file
			        
			        if (line == sectionLabel) {                        // check if section has been entered
                        inSection = true;
                        updated += line + '\n';
                        getline (filePtrI, line);
                    }
                    
                    if (inSection) {
                        if (line[0] == '[') {                          // check for end of section
                            inSection = false;
                            
                            // if data was not replaced in the section, it must be added
                            if (!dataAdded) {
                                updated += key + '=' + data + "\n\n";
                                dataAdded = true;
                            }
                            
                            temp = line + '\n';
                        } else {
        			        linekey = getKeyFromLine(line);            // get the key from the line
                            
                            if (linekey == key) {                      // check if the key matches
                                temp = key + '=' + data + '\n';        // update the key's data
                                dataAdded = true;
                            } else
                                temp = line + '\n';                    // use original data
                        }
                    } else
                        temp = line + '\n';                            // use original data
                    
			        if (temp == "\n") newLinePending = true;
			        else updated += temp;                              // add the line to the contents
		        }
		        
		        if (!dataAdded) {
                    updated += key + '=' + data + "\n";
                    dataAdded = true;
                }
		        
                filePtrI.close();                                      // close the file
                ofstream filePtrO (fileName.c_str(), ios::trunc);      // open the file, erasing all of its contents
                
                // write contents back to file
                if (filePtrO.is_open()) {
                    filePtrO << updated;                               // add the updated contents to the file
                    filePtrO.close();                                  // close the file
                }
            }
        } else {
            ofstream filePtr (fileName.c_str(), ios::app);             // open the file, positioning the write pointer at the end
            if (filePtr.is_open()) {
                filePtr << "\n[" << section << ']' << '\n';            // add the new section to the file
                filePtr << key << '=' << data << '\n';                 // add the new data to the file
                filePtr.close();                                       // close the file
            }
        }
    }
}


string iniFile::getKeyFromLine (string line) {
    int epos;                                                          // position of '=' in the line
    string key = "";                                                   // key text from the line
    
    epos = line.find("=", 0);                                          // find where '=' is in the line
    
    // if '=' exists in the line then all text before it must be the key
    if (epos>0)
        key = line.substr(0,epos);                                     // get the key from the line
    
    return key;                                                        // return the key's value
}


string iniFile::getDataFromLine (string line) {
    int epos;                                                          // position of '=' in the line
    string data = "";                                                  // data text from the line
    
    epos = line.find("=", 0);                                          // find where '=' is in the line
    
    // if '=' exists in the line then all text after it must be the data
    if (epos>0)
        data = line.substr(epos+1);                                    // get the data from the line
    
    return data;                                                       // return the data value
}


int iniFile::sectionStartsAt (string section) {
    int lineAt = 0;
    bool exists = false;
    string sectionLabel ('[' + section + ']');
    string line;
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());                           // open the file for reading
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {
                getline (filePtr, line);
                lineAt++;
                
                if (line == sectionLabel) {
                    exists = true;
                    break;
                }
            }
        }
            
        filePtr.close();                                               // close the file
    }
    
    if (exists) return lineAt;
    else return -1;
}
