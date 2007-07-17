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
    Coords are used to store the position in an image of a point in an Region of
    Interest entity.
*/
struct coords {
    int x;  // x-axis or horizontal position
    int y;  // y-axis of vertical position
};


/**
    An ROIEntity stores a single entity of a Region of Interest. The entity
    stores its type and the coords that it consists of. The type indicates what
    shape the entity is, such as a point, rectangle or polygon. The number of
    coords in the vector is related to the type of entity. A point will have 1
    coord, a rectangle will have 2 and a polygon will have 3 or more.
*/
class ROIEntity {
    public:
        const char* type;       // the type of the entity (POINT, RECT, POLY)
        vector<coords> points;  // the list of coords that make up the entity
        
        
        /**
            Returns the number of coords in the vector. This can be used when
            traversing the coords vector.
        */
        int point_count () {
            return points.size();
        }
        
        
        /**
            Sets the type of the entity from a string that contains the
            appropriate name. This is used when reading Regions of Interest from
            a file. The data read from the file is in string form and this
            function converts that string back to the right type.
        */
        void set_type (string s) {
            if (s=="POINT") type = ROI_POINT;
            else if (s=="RECT") type = ROI_RECT;
            else type = ROI_POLY;
        }
        
        
        /**
            Returns the type of entity. The returned value can be compared to
            global variables ROI_POINT, ROI_RECT and ROI_POLY to find out what
            it is. The type returned is a const char* so it is easy to convert
            to a string to store in a file.
        */
        const char* get_type() {
            return type;
        }
        
        
        /**
            Returns the vector of coords that makes up the entity.
        */
        vector<coords> get_points() {
            return points;
        }
};


/**
    ROI stores all the information to do with a Region of Interest. This
    includes its colour, name and a vector of entities that is consists of.
*/
class ROI {
    private:
        bool active;                  // true if the ROI is visible, false if it is not
        
        int colour_red;               // RGB colour values of the ROI (0-255)
        int colour_green;
        int colour_blue;
        
        string name;                  // name to identify the ROI
        
        vector<ROIEntity*> entities;  // entities that make up this ROI
        
        
    public:
        /**
            Creates a Region of Interest that is visible, has no name and is set
            to the default colour (grey).
        */
        ROI (void);
        
        /**
            Destructor clears the vector of all the entities that make up this
            Region of Interest.
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
            Sets whether the ROI is currently active
        */
        void set_active (bool isActive) { active = isActive; }
        
        /**
            Returns whether the ROI is currently active
        */
        bool get_active (void) { return active; }
        
        
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
