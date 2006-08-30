#include "ROISet.h"

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
void ROISet::new_entity (string t) {
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
    }
}

void ROISet::backtrack (void) {
    if (current_entity!=NULL) {
        vector<coords> *p;
        p = &(current_entity->points);
        if (!p->empty())
            p->pop_back();
//            p->erase(p->end()-2, p->end()-1);
    }
}


bool ROISet::editing () {
    return editingEntity;
}


/* Regions Of Interest ********************************************************/
void ROISet::add_entity_to_current (void) {
    if (current_region!=NULL && current_entity!=NULL)
        current_region->add_entity(*current_entity);
    
    editingEntity = false;
}


ROI* ROISet::new_region (void) {
    return new_region("default");
}

ROI* ROISet::new_region (string n) {
    if (current_region!=NULL) delete current_region;
    current_region = new ROI();
    current_region->set_name(n);
    return current_region;
}


void ROISet::set_current (ROI* c) {
    current_region = c;
}

void ROISet::set_current (string c) {
    for (int i=0; i<regions.size(); i++) {
        ROI r = regions.at(i);
        if (r.get_name() == c) {
            current_region = &r;
            break;
        }
    }
}


/* Set ************************************************************************/
void ROISet::add_current_to_set (void) {
    if (current_region!=NULL)
        regions.push_back(*current_region);
}


void ROISet::remove_region (ROI* to_go) {
    for (int i=0; i<regions.size(); i++) {
        ROI r = regions.at(i);
        if (&r == to_go) {
            regions.erase(regions.begin()+i-1, regions.begin()+i);
            break;
        }
    }
}

void ROISet::remove_region (string to_go) {
    for (int i=0; i<regions.size(); i++) {
        ROI r = regions.at(i);
        if (r.get_name() == to_go) {
            regions.erase(regions.begin()+i-1, regions.begin()+i);
            break;
        }
    }
}


vector<ROI> ROISet::get_regions (void) {
    return regions;
}


int ROISet::get_regions_count (void) {
    return regions.size();
}


bool ROISet::name_exists (string name) {
    bool exists = false;
    
    for (int i=0; i<regions.size(); i++) {
        ROI r = regions.at(i);
        if (r.get_name() == name) {
            exists = true;
            break;
        }
    }
    
    return exists;
}
