#include "RoI.h"

using namespace std;


RoISet::RoISet (void) {
    current_region = NULL;
    new_entity = NULL;
}

RoISet::~RoISet (void) {
    delete &regions;
    delete current_region;
    delete new_entity;
}


void RoISet::new_region (void) {
    if (current_region!=NULL) delete current_region;
    current_region = new RoI();
}

void RoISet::new_region (string n) {
    if (current_region!=NULL) delete current_region;
    current_region = new RoI();
    current_region->set_name(n);
}


void RoISet::start_new (string t) {
    if (new_entity!=NULL) delete new_entity;
    new_entity = new RoIEntity();
    new_entity->type = t;
}


void RoISet::add_point (int x, int y) {
    if (new_entity!=NULL) {
        coords p;
        p.x = x;
        p.y = y;
        (new_entity->points).push_back(p);
    }
}

void RoISet::backtrack (void) {
    if (new_entity!=NULL) {
        vector<coords> *p;
        p = &(new_entity->points);
        if (!p->empty())
            p->erase(p->end()-2, p->end()-1);
    }
}


void RoISet::add_new_to_current (void) {
    if (current_region!=NULL && new_entity!=NULL)
        current_region->add_entity(new_entity);
}


vector<RoI*> RoISet::get_regions (void) {
    return regions;
}


void RoISet::set_current (RoI* c) {
    current_region = c;
}

void RoISet::set_current (string c) {
    for (int i=0; i<regions.size(); i++) {
        RoI* r = regions.at(i);
        if (r->get_name() == c) {
            current_region = r;
            break;
        }
    }
}


void RoISet::delete_region (RoI* to_go) {
    for (int i=0; i<regions.size(); i++) {
        RoI* r = regions.at(i);
        if (r == to_go) {
            regions.erase(regions.begin()+i-1, regions.begin()+i);
            break;
        }
    }
}
