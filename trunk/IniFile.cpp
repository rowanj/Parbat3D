#include "IniFile.h"

using namespace std;


void iniFile::open (string fn) {
    if (fn != "") {           // check that a filename has been passed in
        gotFileName = true;   // show that a filename has been set
        fileName = fn;        // store the filename
    } else
        gotFileName = false;  // no filename passed in so show nothing is stored
}


void iniFile::close () {
    gotFileName = false;      // shows that a filename has not been set
    fileName = "";            // store the filename as blank
}


string iniFile::getFileName () {
    return fileName;
}


bool iniFile::parse (string section, string key) {
    bool keyInFile = false;                     // shows if key has been found in the file
    string line, linekey;                       // temp storage for each line input from file
    string sectionLabel ('[' + section + ']');  // section text as it appears in the file
    bool inSection = false;                     // true if inside the proper section
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());    // open the file for reading
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {
                getline (filePtr, line);        // read in each line of the file
                
                if (!inSection) {
                    if (line == sectionLabel)   // check for entering section
                        inSection = true;
                } else {
                    if (line[0] == '[')         // check for end of section
                        break;
                    else {
                        linekey = getKeyFromLine(line);    // get the key from the line
                        if (linekey == key) {              // check if the key matches
                            keyInFile = true;              // show that the key has been found
                            break;
                        }
                    }
                }
            }

            filePtr.close();                    // close the file
        }
    }
    
    return keyInFile;                           // return true if the key exists in the file
}


string iniFile::read (string section, string key) {
    string data = "";                           // stores the data that corresponds to the key
    string line, linekey;                       // temp storage for each line input from file
    string sectionLabel ('[' + section + ']');  // section text as it appears in the file
    bool inSection = false;                     // true if inside the proper section
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());    // open the file for reading
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {            // find the appropriate section
                getline (filePtr, line);        // read in each line of the file
                
                if (!inSection) {
                    if (line == sectionLabel)   // check for entering section
                        inSection = true;
                } else {
                    if (line[0] == '[')         // check for end of section
                        break;
                    else {
                        linekey = getKeyFromLine(line);    // get the key from the line
                        if (linekey == key) {              // check if the key matches
                            data = getDataFromLine(line);  // store the data from the key
                            break;
                        }
                    }
                }
            }
            
            filePtr.close();                    // close the file
        }
    }
    
    return data;
}


void iniFile::update (string section, string key, string data) {
    string line, linekey;                           // temp storage for each line input from file
    string sectionLabel ('[' + section + ']');      // section text as it appears in the file
    bool inSection = false;                         // true if the file pointer is in the desired section or not
    bool dataAdded = false;                         // true if the file has been added/updated with the data
    string newLinesToAdd = "";                      // stores how many new lines are pending to be added
    
    if (gotFileName) {
        ifstream filePtrI (fileName.c_str());       // open the original file for reading
        ofstream filePtrO (".temp", ios::trunc);    // create temp file, erase contents if any
        
        if (filePtrI.is_open() && filePtrO.is_open()) {
            while (!filePtrI.eof()) {               // loop through the entire file
                getline(filePtrI, line);            // read in each line of the file
                
                if (line=="") {                     // store the extra new lines
                    newLinesToAdd += "\n";
                
                } else {
                    // before and after the desired section
                    if (!inSection) {
                        if (line == sectionLabel)
                            inSection = true;
                        
                        filePtrO << newLinesToAdd;
                        newLinesToAdd = "";
                        filePtrO << line << '\n';
                    
                    // while inside the section
                    } else {
                        // check to see if at end of section
                        if (line[0] == '[') {
                            // if data was not replaced in the section, it must be added
                            if (!dataAdded) {
                                filePtrO << key << '=' << data << '\n';
                                dataAdded = true;
                            }
                            
                            filePtrO << newLinesToAdd;
                            newLinesToAdd = "";
                            filePtrO << line << '\n';
                            
                            inSection = false;
                        
                        } else {
                            filePtrO << newLinesToAdd;
                            newLinesToAdd = "";
                            
                            // check if it is the line to replace
        			        linekey = getKeyFromLine(line);              // get the key from the line
                            if (linekey == key) {                        // check if the key matches
                                filePtrO << key << '=' << data << '\n';  // update the key's data
                                dataAdded = true;
                            } else
                                filePtrO << line << '\n';
                        }
                    }
                }
            }

            if (!dataAdded) {
                // if the section does not already exist it must be added
                if (!inSection)
                    filePtrO << '\n' << sectionLabel << '\n';   // add the new section to the file
                filePtrO << key << '=' << data << '\n';         // add the data to the section
                dataAdded = true;
            }
            
            filePtrI.close();                                   // close the reading file
            filePtrO.close();                                   // close the writing file
            
            // replace the original file with the updated temp one
            remove(fileName.c_str());
            rename(".temp", fileName.c_str());
        
        // both files could not be opened together (but one may have been)
        } else {
            // input file isn't open but output is (original doesn't exist)
            if (!filePtrI.is_open() && filePtrO.is_open()) {
                filePtrO.close();  // clean up since file cannot be opened
                remove(".temp");
                
                // add the data to the end of the file (works if file doesn't exist)
                ofstream filePtrO (fileName.c_str(), ios::app);
                if (filePtrO.is_open()) {
                    filePtrO << sectionLabel << '\n';
                    filePtrO << key << '=' << data << '\n';
                    filePtrO.close();
                }
            }
        }
    }
}


string iniFile::getKeyFromLine (string line) {
    int epos;                        // position of '=' in the line
    string key = "";                 // key text from the line
    
    epos = line.find("=", 0);        // find where '=' is in the line
    
    // if '=' exists in the line then all text before it must be the key
    if (epos>0)
        key = line.substr(0,epos);   // get the key from the line
    
    return key;                      // return the key's value
}


string iniFile::getDataFromLine (string line) {
    int epos;                        // position of '=' in the line
    string data = "";                // data text from the line
    
    epos = line.find("=", 0);        // find where '=' is in the line
    
    // if '=' exists in the line then all text after it must be the data
    if (epos>0)
        data = line.substr(epos+1);  // get the data from the line
    
    return data;                     // return the data value
}


int iniFile::sectionStartsAt (string section) {
    int lineAt = 0;
    bool exists = false;
    string sectionLabel ('[' + section + ']');
    string line;
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());
        
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
            
        filePtr.close();
    }
    
    if (exists) return lineAt;
    else return -1;
}


bool iniFile::sectionExists(string section) {
    return (sectionStartsAt(section)>0);
}


void iniFile::removeSection(string section) {
    removeSection(section, false);
}

void iniFile::removeSection(string section, bool keepTitle) {
    string sectionLabel ('[' + section + ']');
    string line;
    int phase = 0;
    
    ifstream filePtrI (fileName.c_str());     // open the original file for reading
    ofstream filePtrO (".temp", ios::trunc);  // create temp file, erase contents if any
    
    if (filePtrI.is_open() && filePtrO.is_open()) {
        while (!filePtrI.eof()) {
            getline(filePtrI, line);
            
            // enter the section to remove
            if (phase==0 && line==sectionLabel) {
                phase = 1;
                if (keepTitle) filePtrO << line << '\n';
                
            // while inside the section
            } else if (phase==1) {
                // if the end of the section is reached
                if (line[0]=='[') {
                    phase = 2;
                    filePtrO << line << '\n';
                }
                
            // before and after the section
            } else
                filePtrO << line << '\n';
        }
        
        filePtrI.close();  // close the reading file
        filePtrO.close();  // close the writing file
        
        // replace the original file with the updated temp one
        remove(fileName.c_str());
        rename(".temp", fileName.c_str());
    }
}
