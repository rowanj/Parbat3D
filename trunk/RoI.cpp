#include "RoI.h"

using namespace std;


RoI::RoI (void) {
}

RoI::~RoI (void) {
}


void RoI::set_name (string n) {
    name = n;
}

string RoI::get_name (void) {
    return name;
}


void RoI::set_color (int r, int g, int b) {
    color_red = r;
    color_green = g;
    color_blue = b;
}

void RoI::get_color (int* r, int* g, int* b) {
    r = &color_red;
    g = &color_green;
    b = &color_blue;
}


void RoI::add_entity (RoIEntity* e) {
    entities.push_back(e);
}

vector<RoIEntity*> RoI::get_entities (void) {
    return entities;
}
