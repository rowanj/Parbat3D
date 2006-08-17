#include "RoI.h"

using namespace std;


RoI::RoI (void) {
    active = true;
    colour_red = 0;
    colour_green = 0;
    colour_blue = 0;
}

RoI::~RoI (void) {
}


void RoI::set_name (string n) {
    name = n;
}

string RoI::get_name (void) {
    return name;
}


void RoI::set_color (int r, int g, int b) { set_colour(r,g,b); }
void RoI::set_colour (int r, int g, int b) {
    colour_red = r;
    colour_green = g;
    colour_blue = b;
}

void RoI::get_color (int* r, int* g, int* b) { get_colour(r,g,b); }
void RoI::get_colour (int* r, int* g, int* b) {
    *r = colour_red;
    *g = colour_green;
    *b = colour_blue;
}


void RoI::add_entity (RoIEntity* e) {
    entities.push_back(e);
}

vector<RoIEntity*> RoI::get_entities (void) {
    return entities;
}
