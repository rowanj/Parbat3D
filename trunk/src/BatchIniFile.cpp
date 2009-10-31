#include "PchApp.h"

#include "BatchIniFile.h"

using namespace std;


string BatchIniFile::getBuffer () {
    return *contentBuffer;
}


void BatchIniFile::clearBuffer () {
    contentBuffer = new string("");
}


void BatchIniFile::updateBuffer (string key, string data) {
    string toAdd (key + '=' + data + '\n');
    string* newContentBuffer = new string ("");
    char *line;
    string *linekey;
    bool dataUpdated = false;
    
    line = strtok(copyString(contentBuffer->c_str()), "\n");
    while (line != NULL) {
        linekey = new string (line, key.size());
        
        if (linekey->compare(key)==0) {
            newContentBuffer->append(toAdd.c_str());
            dataUpdated = true;
        } else {
	  if (string("") != line) newContentBuffer->append(line);
            newContentBuffer->append("\n");
        }
        
        line = strtok(NULL, "\n");
    }
    
    if (!dataUpdated)
        newContentBuffer->append(toAdd.c_str());
    
    /* TODO (#1#): Wondering if I should free contentBuffer here */
    
    contentBuffer = newContentBuffer;
}


string BatchIniFile::readFromBuffer (string key) {
    string data;      // data value of the key
    char *line;       // current line from the buffer
    string *lineS;    // variable "line" as a string
    string *linekey;  // part of current line to compare to key
    
    line = strtok(copyString(contentBuffer->c_str()), "\n");
    while (line != NULL) {
        linekey = new string (line, key.size());
        if (linekey->compare(key) == 0) break;
        line = strtok(NULL, "\n");
    }
    
    if (line == NULL)
        data = "";
    else {
        lineS = new string (line);
        data = lineS->substr(key.size()+1);
    }
    
    return data;
}


void BatchIniFile::writeBufferToSection (string section) {
    writeBufferToSection(section, false);
}

void BatchIniFile::writeBufferToSection (string section, bool saveContents) {
    string sectionLabel ('[' + section + ']');
    bool inSection = false;
    string line;
    bool addedBuffer = false;
    
    if (gotFileName) {
        ifstream filePtrI (fileName.c_str());     // open the original file for reading
        ofstream filePtrO (".temp", ios::trunc);  // create temp file, erase contents if any
        
        if (filePtrI.is_open() && filePtrO.is_open()) {
            while (!filePtrI.eof()) {             // loop through the entire file
                getline(filePtrI, line);          // read in each line of the file
                
                // while not in section
                if (!inSection) {
                    // just entered section
                    if (line == sectionLabel)
                        inSection = true;
                    
                    if (line=="") filePtrO << '\n';
                    else filePtrO << line << '\n';
                    
                // while in section
                } else {
                    // leaving section
                    if (line[0] == '[') {
                        filePtrO << *contentBuffer;
                        filePtrO << line << '\n';
                        addedBuffer = true;
                        inSection = false;
                        
                    // inside section
                    } else {
                        if (saveContents) {
                            if (line=="") filePtrO << '\n';
                            else filePtrO << line << '\n';
                        }
                    }
                }
            }
            
            // buffer was not added
            if (!addedBuffer) {
                if (!inSection)
                    filePtrO << sectionLabel << '\n';
                
                filePtrO << *contentBuffer;
            }
            
            filePtrI.close();  // close the reading file
            filePtrO.close();  // close the writing file
            
            // replace the original file with the updated temp one
            remove(fileName.c_str());
            rename(".temp", fileName.c_str());
            
        // both files (original and/or temp) cannot be opened
        } else {
            // the original file cannot be opened but the temp file could
            if (!filePtrI.is_open() && filePtrO.is_open()) {
                filePtrO.close();  // clean up since file cannot be opened
                remove(".temp");
                
                // add the data to the end of the file (works if file doesn't exist)
                ofstream filePtrO (fileName.c_str(), ios::app);  // or ios::trunc to remove previous
                if (filePtrO.is_open()) {
                    filePtrO << sectionLabel << '\n';
                    filePtrO << *contentBuffer;
                    filePtrO.close();
                }
            }
        }
    }
}


string BatchIniFile::readSectionContent (string section) {
    string sectionLabel ('[' + section + ']');
    bool inSection = false;
    string line;
    
    clearBuffer();
    
    if (gotFileName) {
        ifstream filePtr (fileName.c_str());
        
        if (filePtr.is_open()) {
            while (!filePtr.eof()) {
                getline (filePtr, line);
                
                if (!inSection) {
                    if (line == sectionLabel)
                        inSection = true;
                } else {
                    if (line[0] == '[')
                        inSection = false;
                    else {
                        if (line!="") contentBuffer->append(line);
                        contentBuffer->append("\n");
                    }
                }
            }
            
            filePtr.close();
        }
    }
    
    return *contentBuffer;
}
