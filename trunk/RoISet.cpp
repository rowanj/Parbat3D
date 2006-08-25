#include "RoI.h"

using namespace std;


RoISet::RoISet () {
    current_region = NULL;
    current_entity = NULL;
}

RoISet::~RoISet (void) {
    delete &regions;
    delete current_region;
    delete current_entity;
}


/* Entities *******************************************************************/
void RoISet::new_entity (string t) {
    if (current_entity!=NULL) delete current_entity;
    current_entity = new RoIEntity();
    current_entity->type = t;
}


void RoISet::add_point (int x, int y) {
    if (current_entity!=NULL) {
        coords p;
        p.x = x;
        p.y = y;
        (current_entity->points).push_back(p);
    }
}

void RoISet::backtrack (void) {
    if (current_entity!=NULL) {
        vector<coords> *p;
        p = &(current_entity->points);
        if (!p->empty())
            p->pop_back();
//            p->erase(p->end()-2, p->end()-1);
    }
}


/* Regions Of Interest ********************************************************/
void RoISet::add_entity_to_current (void) {
    if (current_region!=NULL && current_entity!=NULL)
        current_region->add_entity(*current_entity);
}


RoI* RoISet::new_region (void) {
    return new_region("default");
}

RoI* RoISet::new_region (string n) {
    if (current_region!=NULL) delete current_region;
    current_region = new RoI();
    current_region->set_name(n);
    return current_region;
}


void RoISet::set_current (RoI* c) {
    current_region = c;
}

void RoISet::set_current (string c) {
    for (int i=0; i<regions.size(); i++) {
        RoI r = regions.at(i);
        if (r.get_name() == c) {
            current_region = &r;
            break;
        }
    }
}


/* Set ************************************************************************/
void RoISet::add_current_to_set (void) {
    if (current_region!=NULL)
        regions.push_back(*current_region);
}


void RoISet::remove_region (RoI* to_go) {
    for (int i=0; i<regions.size(); i++) {
        RoI r = regions.at(i);
        if (&r == to_go) {
            regions.erase(regions.begin()+i-1, regions.begin()+i);
            break;
        }
    }
}

void RoISet::remove_region (string to_go) {
    for (int i=0; i<regions.size(); i++) {
        RoI r = regions.at(i);
        if (r.get_name() == to_go) {
            regions.erase(regions.begin()+i-1, regions.begin()+i);
            break;
        }
    }
}


vector<RoI> RoISet::get_regions (void) {
    return regions;
}


int RoISet::get_regions_count (void) {
    return regions.size();
}


bool RoISet::name_exists (string name) {
    bool exists = false;
    
    for (int i=0; i<regions.size(); i++) {
        RoI r = regions.at(i);
        if (r.get_name() == name) {
            exists = true;
            break;
        }
    }
    
    return exists;
}
