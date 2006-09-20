#include "ROISet.h"

#include "StringUtils.h"


using namespace std;


ROISet::ROISet () {
    current_region = NULL;
    current_entity = NULL;
    editingEntity = false;
}

ROISet::~ROISet (void) {
    delete &regions;
    delete current_region;
    delete current_entity;
}


/* Entities *******************************************************************/
void ROISet::new_entity (const char* t) {
    if (current_entity!=NULL) delete current_entity;
    current_entity = new ROIEntity();
    current_entity->type = t;
    editingEntity = true;
}


void ROISet::add_point (int x, int y) {
    if (current_entity!=NULL) {
        coords p;
        p.x = x;
        p.y = y;
        (current_entity->points).push_back(p);
        
        if ((current_entity->type==ROI_POINT && (current_entity->points).size()==1) ||
            (current_entity->type==ROI_RECT  && (current_entity->points).size()==2)) {
            finish_entity();
        }
    }
}

void ROISet::backtrack (void) {
    if (current_entity!=NULL) {
        vector<coords> *p;
        p = &(current_entity->points);
        if (p->size()>1)
            p->pop_back();
//            p->erase(p->end()-2, p->end()-1);
        else
            finish_entity(false);
    }
}


void ROISet::finish_entity (void) {
    finish_entity(true);
}

void ROISet::finish_entity (bool keep) {
    if (current_region!=NULL && current_entity!=NULL) {
        // checks if user wants to keep it and if it can be kept
        if (keep &&
            (current_entity->type!=ROI_POINT || current_entity->point_count()==1) &&
            (current_entity->type!=ROI_RECT  || current_entity->point_count()==2) &&
            (current_entity->type!=ROI_POLY  || current_entity->point_count()>=3))
            current_region->add_entity(current_entity);
        else
            delete current_entity;
        
        current_entity = NULL;
    }
    
    editingEntity = false;
}


bool ROISet::editing () {
    return editingEntity;
}

const char* ROISet::editingType () {
    if (current_entity!=NULL)
        return current_entity->type;
    else
        return ROI_NONE;
}

ROIEntity* ROISet::get_current_entity()
{
	return current_entity;
}

/* Regions Of Interest ********************************************************/
ROI* ROISet::new_region (void) {
    // get unique name
    int listSize = regions.size();
    int i = listSize;
    
    string *name = new string(makeMessage("ROI-", i));
    while (name_exists(*name)) {
        i++;
        name = new string(makeMessage("ROI-", i));
    }
    
    // create and return new region
    return new_region(*name);
}


ROI* ROISet::new_region (string n) {
    current_region = NULL;
    
    current_region = new ROI();         // creates the new ROI
    current_region->set_name(n);        // gives ROI its name
    regions.push_back(current_region);  // adds the ROI to the set
    
    return current_region;              // this ROI is returned in case the user wants it
}


void ROISet::set_current (string c) {
    for (int i=0; i<regions.size(); i++) {
        ROI* r = regions.at(i);
        if (r->get_name() == c) {
            current_region = r;
            break;
        }
    }
}


/* Set ************************************************************************/
void ROISet::add_region_to_set (ROI* roi, bool replace) {
    string name = roi->get_name();
    
    if (!name_exists(name))
        regions.push_back(roi);
    else {
        if (replace) {
            remove_region(name);
            regions.push_back(roi);
        }
    }
}


void ROISet::remove_region (ROI* to_go) {
    for (int i=0; i<regions.size(); i++) {
        ROI* r = regions.at(i);
        if (r == to_go) {
            regions.erase(regions.begin()+i, regions.begin()+i+1);
            break;
        }
    }
}

void ROISet::remove_region (string to_go) {
    for (int i=0; i<regions.size(); i++) {
        ROI* r = regions.at(i);
        if (r->get_name() == to_go) {
            regions.erase(regions.begin()+i, regions.begin()+i+1);
            break;
        }
    }
}


vector<ROI*> ROISet::get_regions (void) {
    return regions;
}


int ROISet::get_regions_count (void) {
    return regions.size();
}


bool ROISet::name_exists (string name) {
    bool exists = false;
    
    for (int i=0; i<regions.size(); i++) {
        ROI* r = regions.at(i);
        if (r->get_name() == name) {
            exists = true;
            break;
        }
    }
    
    return exists;
}


void ROISet::combine(ROISet* rs, bool replace) {
    vector<ROI*> rlist = rs->get_regions();
    
    for (int i=0; i<rlist.size(); i++) {
        ROI* roi = rlist.at(i);           // grabs an ROI from the new set
        add_region_to_set(roi, replace);  // adds it to the current one
    }
}
