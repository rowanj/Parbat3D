#include "ROI.h"

using namespace std;


const char* ROI_POINT = "POINT";
const char* ROI_RECT = "RECT";
const char* ROI_POLY = "POLY";


ROI::ROI (void) {
    active = true;
    colour_red = 0;
    colour_green = 0;
    colour_blue = 0;
}

ROI::~ROI (void) {
}


void ROI::set_name (string n) {
    name = n;
}

string ROI::get_name (void) {
    return name;
}


void ROI::set_color (int r, int g, int b) { set_colour(r,g,b); }
void ROI::set_colour (int r, int g, int b) {
    colour_red = r;
    colour_green = g;
    colour_blue = b;
}

void ROI::get_color (int* r, int* g, int* b) { get_colour(r,g,b); }
void ROI::get_colour (int* r, int* g, int* b) {
    *r = colour_red;
    *g = colour_green;
    *b = colour_blue;
}


void ROI::add_entity (ROIEntity* e) {
    entities.push_back(e);
}


vector<ROIEntity*> ROI::get_entities (void) {
    return entities;
}
