#ifndef _ROI_H
#define _ROI_H

#include <vector>
#include <string>

using namespace std;


// these represent the different types of entities that can be created
extern const char* ROI_NONE;   // not an ROI (used for checking if one is being edited)
extern const char* ROI_POINT;  // consist of a single coord
extern const char* ROI_RECT;   // consist of 2 coords (top-left and botton-right)
extern const char* ROI_POLY;   // consist of 3 or more coords


/**
    Used to store a position of a point in an Region of Interest entity.
*/
struct coords {
    int x;  // x-axis or horizontal position
    int y;  // y-axis of vertical position
};


/**
    Regions of Interest are made up of a list of entities. The entity stores its
    type and the points that make it up.
*/
class ROIEntity {
    public:
        const char* type;       // the type of the entity (POINT, RECT, POLY)
        vector<coords> points;  // the list of coords that make up the entity
        
        int point_count () {
            return points.size();
        }
        
        void set_type (string s) {
            if (s=="POINT") type = ROI_POINT;
            else if (s=="RECT") type = ROI_RECT;
            else type = ROI_POLY;
        }
};


/**
    A Region of Interest has a colour, a name and a list of entities.
*/
class ROI {
    private:
        bool active;  // make not be used - remove later if this is the case
        int colour_red;               // RGB colour values of the ROI
        int colour_green;
        int colour_blue;
        string name;                  // name to identify the ROI
        vector<ROIEntity*> entities;  // entities that make up this ROI
        
    public:
        /**
            Creates Regions of Interest with default colour (black).
        */
        ROI (void);
        
        /**
            Default destructor - does nothing.
        */
        ~ROI (void);
        
        
        /**
            Sets the name of the Region of Interest. This name must be unique
            for all Regions of Interest in a set.
        */
        void set_name (string);
        
        /**
            Returns the name of this Region of Interest.
        */
        string get_name (void);
        
        
        /**
            Sets the colour of this Region of Interest based on red, green and
            blue values that are passed in.
        */
        void set_colour (int, int, int);
        
        /**
            Does the same a set_colour() but is here to allow American spelling.
        */
        void set_color (int, int, int);
        
        
        /**
            Allows the user to get the 3 colours that make up this Region of
            Interest by sending in pointers they want the values to be set to.
        */
        void get_colour (int*, int*, int*);
        
        /**
            Does the same a get_colour() but is here to allow American spelling.
        */
        void get_color (int*, int*, int*);
        
        
        /**
            Adds an entity to this Region of Interest.
        */
        void add_entity (ROIEntity*);
        
        
        /**
            Returns a vector consisting of all the entities that make up this
            Region of Interest.
        */
        vector<ROIEntity*> get_entities (void);
};

#endif
