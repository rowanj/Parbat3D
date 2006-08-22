#include "RoIFile.h"
#include "StringUtils.h"


using namespace std;


RoIFile::RoIFile () {}

RoIFile::~RoIFile() {}


void RoIFile::open (string filename) {
    openFile.open(filename);
}

void RoIFile::close () {
    openFile.close();
}


/* Set ************************************************************************/
void RoIFile::saveSetToFile (string filename, RoISet* rs) {
    openFile.open(filename);
    saveSetToFile(rs);
}

void RoIFile::saveSetToFile (RoISet* rs) {
    vector<RoI> rlist = rs->get_regions();
    RoI roi;
    
    for (int i=0; i<rlist.size(); i++) {
        roi = rlist.at(i);
        saveRegionToFile(&roi);
    }
}


RoISet* RoIFile::loadSetFromFile (string filename) {
    RoISet* rs;
    openFile.open(filename);
    rs = loadSetFromFile();
    return rs;
}

RoISet* RoIFile::loadSetFromFile () {
    RoISet* rset = new RoISet();
    RoI* roi;
    string roiCountStr;
    int roiCount;
    string roiName;
    string roiKey;
    
    // get the number of ROIs in the file
    roiCountStr = openFile.read("ROI List","count");
    roiCount = stringToInt(roiCountStr);
    
    // load the ROIs one at a time
    for (int i=0; i<roiCount; i++) {
        roiKey = makeMessage("roi ", i);              // create the key for each ROI
        roiName = openFile.read("ROI List", roiKey);  // get the name of the ROI
        roi = loadRegionFromFile(roiName);            // load the ROI from the file
        rset->set_current(roi);                       // add it to the current set
        rset->add_current_to_set();                   // update the set
    }
    
    return rset;
}


/* Region *********************************************************************/
void RoIFile::saveRegionToFile (string filename, RoI* roi) {
    openFile.open(filename);
    saveRegionToFile(roi);
}

void RoIFile::saveRegionToFile (RoI* roi) {
    string section;             // name of section
    vector<RoIEntity> elist;    // list of entities
    RoIEntity re;               // current entity working on
    vector<coords> pts;         // list of points
    coords pt;                  // current point working on
    int rcol, gcol, bcol;       // storage for colours
    string colStr;              // colour string (for file)
    int totalEntities;
    string et;                  // type of entity
    int totalPoints;
    string ep, epc;             // name of point
    string curPtXY;
    string totalRegionsStr;
    int totalRegions;
    string leader = "";         // blank leader used for creating strings
    
    section = roi->get_name();
    
    // if the ROI already exists in the file, remove its data from file
    if (openFile.sectionExists(section))
        openFile.removeSection(section, true);
    else {
        // add the new section info to the list
        totalRegionsStr = openFile.read("ROI List","count");
        if (totalRegionsStr == "") {
            openFile.update("ROI List", "count", "1");
            openFile.update("ROI List", "roi 0", section);
        } else {
            stringstream ss;
            ss << totalRegionsStr;
            ss >> totalRegions;
            openFile.update("ROI List",
                            makeString("roi ", totalRegions),
                            section);
            totalRegions++;
            openFile.update("ROI List",
                            "count",
                            makeString(leader, totalRegions));
        }
    }
    
    // save ROI colours
    roi->get_colour(&rcol,&gcol,&bcol);
    colStr = makeString("", rcol);
    colStr += ' '; colStr = makeString(colStr, gcol);
    colStr += ' '; colStr = makeString(colStr, bcol);
    openFile.update(section,"colour",colStr);
    
    // save ROI entities
    elist = roi->get_entities();
    totalEntities = elist.size();
    openFile.update(section,"entities", makeString(leader, totalEntities));
    for (int i=0; i<totalEntities; i++) {
        re = elist.at(i);
        et = makeString(makeString("entity ", i), " type");
        openFile.update(section, et, re.type);
        
        // save entity points
        pts = re.points;
        ep = makeString(makeString("entity ", i), " point ");
        totalPoints = pts.size();
        openFile.update(section,
                        makeString(makeString("entity ", i), " points"),
                        makeString(leader, totalPoints));
        for (int i=0; i<totalPoints; i++) {
            pt = pts.at(i);
            epc = makeString(ep, i);
            curPtXY = makeString(leader, pt.x);
            curPtXY += ' ';
            curPtXY = makeString(curPtXY, pt.y);
            openFile.update(section,epc,curPtXY);
        }
    }
}


RoI* RoIFile::loadRegionFromFile (string filename, string name) {
    RoI* roi;
    openFile.open(filename);
    roi = loadRegionFromFile(name);
    return roi;
}

RoI* RoIFile::loadRegionFromFile (string name) {
    RoI* roi = new RoI();
    RoIEntity* entityCur;
    char *cTemp, *cTempB;
    string sTemp;
    int rcol, gcol, bcol;  // colours
    int entityCount;       // entity
    string entityBaseStr;
    int pointCount;        // coords
    string pointBaseStr;
    coords pt;
    
    roi->set_name(name);
    
    // get ROI colours from file
    sTemp = openFile.read(name, "colour");
    cTemp = copyString(sTemp.c_str());
    cTempB = strtok(cTemp, " ");            // get red
    rcol = stringToInt(cTempB);
    cTempB = strtok(NULL, " ");             // get green
    gcol = stringToInt(cTempB);
    cTempB = strtok(NULL, " ");             // get blue
    bcol = stringToInt(cTempB);
    roi->set_colour(rcol,gcol,bcol);
    
    entityCount = stringToInt(openFile.read(name, "entities"));
    for (int i=0; i<entityCount; i++) {
        entityBaseStr = makeString("entity ",i);
        entityCur = new RoIEntity();
        entityCur->type = openFile.read(name, makeString(entityBaseStr, " type"));
        
        pointCount = stringToInt(openFile.read(name, makeString(entityBaseStr, " points")));
        for (int j=0; j<pointCount; j++) {
            pointBaseStr = makeString(entityBaseStr, " point ");
            sTemp = openFile.read(name, makeString(pointBaseStr, j));
            cTemp = copyString(sTemp.c_str());
            cTempB = strtok(cTemp, " ");        // get x
            pt.x = stringToInt(cTempB);
            cTempB = strtok(NULL, " ");        // get y
            pt.y = stringToInt(cTempB);
            
            (entityCur->points).push_back(pt);
        }
        
        roi->add_entity(*entityCur);
    }
    
    return roi;
}
