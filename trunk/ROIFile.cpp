#include "ROIFile.h"


using namespace std;


ROIFile::ROIFile () {}

ROIFile::~ROIFile() {}


void ROIFile::open (string filename) {
    openFile.open(filename);
}

void ROIFile::close () {
    openFile.close();
}


/* Set ************************************************************************/
void ROIFile::saveSetToFile (string filename, ROISet* rs, bool append) {
    openFile.open(filename);
    saveSetToFile(rs, append);
}

void ROIFile::saveSetToFile (ROISet* rs, bool append) {
    vector<ROI*> rlist = rs->get_regions();
    ROI* roi;
    
    if (!append)
        openFile.clearFileContents();
    
    for (int i=0; i<rlist.size(); i++) {
        roi = rlist.at(i);
        saveRegionToFile(roi);
    }
}


ROISet* ROIFile::loadSetFromFile (string filename) {
    openFile.open(filename);
    return loadSetFromFile();
}

ROISet* ROIFile::loadSetFromFile () {
    ROISet* rset = new ROISet();
    ROI* roi;
    string roiCountStr;
    int roiCount;
    string roiName;
    string roiKey;
    
    // get the number of ROIs in the file
    roiCountStr = openFile.read("ROI List","count");
    roiCount = stringToInt(roiCountStr);
    
    // load the ROIs one at a time
    for (int i=0; i<roiCount; i++) {
        roiKey = makeMessage("ROI ", i);              // create the key for each ROI
        roiName = openFile.read("ROI List", roiKey);  // get the name of the ROI
        roi = loadRegionFromFile(roiName);            // load the ROI from the file
        rset->add_region_to_set(roi, true);           // add it to the current set
    }
    
    return rset;
}


/* Region *********************************************************************/
void ROIFile::saveRegionToFile (string filename, ROI* roi) {
    openFile.open(filename);
    saveRegionToFile(roi);
}

void ROIFile::saveRegionToFile (ROI* roi) {
    string section;            // name of section
    vector<ROIEntity*> elist;  // list of entities
    ROIEntity* re;             // current entity working on
    vector<coords> pts;        // list of points
    coords pt;                 // current point working on
    int rcol, gcol, bcol;      // storage for colours
    string colStr;             // colour string (for file)
    int totalEntities;
    string et;                 // type of entity
    int totalPoints;
    string ep, epc;            // name of point
    string curPtXY;
    string leader = "";        // blank leader used for creating strings
    
    // set the name of the section based on the name of the  ROI
    section = roi->get_name();
    
    // ROI does not exist, so info must be added
    if (openFile.readSectionContent(section)=="") {
        // there is already a header section in the file
        if (openFile.readSectionContent("ROI List")!="") {
            int totalRegions;
            string totalRegionsStr = openFile.readFromBuffer("count");
            
            if (totalRegionsStr == "")
                totalRegions = 0;
            else {
                stringstream ss;
                ss << totalRegionsStr;
                ss >> totalRegions;
            }
            
            openFile.updateBuffer("count", makeString(leader, totalRegions+1));
            openFile.updateBuffer(makeString("ROI ", totalRegions), section);
            openFile.writeBufferToSection("ROI List");
        
        // a header section must be added to the file
        } else {
            openFile.updateBuffer("count", "1");
            openFile.updateBuffer("ROI 0", section);
            openFile.writeBufferToSection("ROI List");
        }
    }
    
    openFile.clearBuffer();
    
    // save ROI colours
    roi->get_colour(&rcol,&gcol,&bcol);
    colStr = makeString("", rcol);
    colStr += ' '; colStr = makeString(colStr, gcol);
    colStr += ' '; colStr = makeString(colStr, bcol);
    openFile.updateBuffer("colour",colStr);
    
    // save ROI entities
    elist = roi->get_entities();
    totalEntities = elist.size();
    openFile.updateBuffer("entities", makeString(leader, totalEntities));
    for (int i=0; i<totalEntities; i++) {
        re = elist.at(i);
        et = makeString(makeString("entity ", i), " type");
        openFile.updateBuffer(et, makeString(leader, copyString(re->type)));
        
        // save entity points
        pts = re->points;
        ep = makeString(makeString("entity ", i), " point ");
        totalPoints = pts.size();
        openFile.updateBuffer(makeString(makeString("entity ", i), " points"),
                              makeString(leader, totalPoints));
        for (int i=0; i<totalPoints; i++) {
            pt = pts.at(i);
            epc = makeString(ep, i);
            curPtXY = makeString(leader, pt.x);
            curPtXY += ' ';
            curPtXY = makeString(curPtXY, pt.y);
            openFile.updateBuffer(epc, curPtXY);
        }
    }
    
    openFile.writeBufferToSection(section);
}


ROI* ROIFile::loadRegionFromFile (string filename, string name) {
    ROI* roi;
    openFile.open(filename);
    roi = loadRegionFromFile(name);
    return roi;
}

ROI* ROIFile::loadRegionFromFile (string name) {
    ROI* roi = new ROI();    // ROI to return
    ROIEntity* entityCur;    // entity
    int entityCount;
    string entityBaseStr;
    int pointCount;          // coords
    string pointBaseStr;
    coords pt;
    string data;             // stores data that is read from the buffer
    char *dataC, *dataCbit;  // stores the char* values of that data
    int rcol, gcol, bcol;    // colours
    
    // set the name of the ROI
    roi->set_name(name);
    
    // stores the contents of the section in the file buffer
    openFile.readSectionContent(name);
    
    // get ROI colours from file
    data = openFile.readFromBuffer("colour");
    dataC = copyString(data.c_str());
    dataCbit = strtok(dataC, " ");            // get red
    rcol = stringToInt(dataCbit);
    dataCbit = strtok(NULL, " ");             // get green
    gcol = stringToInt(dataCbit);
    dataCbit = strtok(NULL, " ");             // get blue
    bcol = stringToInt(dataCbit);
    roi->set_colour(rcol,gcol,bcol);
    
    entityCount = stringToInt(openFile.readFromBuffer("entities"));
    for (int i=0; i<entityCount; i++) {
        entityBaseStr = makeString("entity ",i);
        entityCur = new ROIEntity();
        entityCur->set_type(openFile.readFromBuffer(makeString(entityBaseStr, " type")));
        
        pointCount = stringToInt(openFile.readFromBuffer(makeString(entityBaseStr, " points")));
        for (int j=0; j<pointCount; j++) {
            pointBaseStr = makeString(entityBaseStr, " point ");
            data = openFile.readFromBuffer(makeString(pointBaseStr, j));
            dataC = copyString(data.c_str());
            dataCbit = strtok(dataC, " ");     // get x
            pt.x = stringToInt(dataCbit);
            dataCbit = strtok(NULL, " ");      // get y
            pt.y = stringToInt(dataCbit);
            
            (entityCur->points).push_back(pt);
        }
        
        roi->add_entity(entityCur);
    }
    
    return roi;
}
