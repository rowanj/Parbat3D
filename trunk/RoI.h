#ifndef _ROI_H
#define _ROI_H

#include <vector>
#include <string>

using namespace std;


struct coords {
    int x;
    int y;
};


class RoIEntity {
    public:
        vector<coords> points;
        string type;
};


class RoI {
    private:
        bool active;
        int colour_red;
        int colour_green;
        int colour_blue;
        string name;
        vector<RoIEntity> entities;
        
    public:
        RoI (void);
        
        virtual ~RoI (void);
        
        void set_name (string);
        
        string get_name (void);
        
        void set_colour (int, int, int);
        void set_color (int, int, int);
        
        void get_colour (int*, int*, int*);
        void get_color (int*, int*, int*);
        
        void add_entity (RoIEntity);
        
        vector<RoIEntity> get_entities (void);
};


class RoISet {
    private:
        vector<RoI> regions;        // the set of all Regions of Interest
        RoI* current_region;        // the Region of Interest currently being worked on
        RoIEntity* current_entity;  // the shape currently being worked on
        
        
    public:
        RoISet ();
        
        virtual ~RoISet (void);
        
        
        /**
            Creates a new shape that will be part of the Region of Interest. The
            parameter passed in specifies what type of shape it will be - POINT,
            RECT, POLY.
        */
        void new_entity (string);
        
        /**
            Adds a point to the shape that is currently being created. If there
            is no shape being creating, nothing happens.
        */
        void add_point (int, int);
        
        /**
            Removes the last point added to the shape that is currently being
            created. If there is no shape being creating or if no points have
            been added, nothing happens.
        */
        void backtrack (void);
        
        
        /**
            Adds the shape that was just created to the Region of Interest.
        */
        void add_entity_to_current (void);
        
        
        /**
            Creates a new Region of Interest. If there is a region that is
            currently be worked on, it is replaced by the new one.
            Returns the newly created region.
        */
        RoI* new_region (void);
        
        /**
            Creates a new Region of Interest, with a specified name. If there is
            a region that is currently be worked on, it is replaced by the new
            one.
            Returns the newly created region.
        */
        RoI* new_region (string);
        
        
        /**
            Sets the Region of Interest to work on. The Region of Interest does
            not have to be in the set.
        */
        void set_current (RoI*);
        
        /**
            Sets the Region of Interest to work on, based on its name. The
            region must already exist in the set otherwise nothing happens.
        */
        void set_current (string);
        
        
        /**
            Adds the current Region of Interest to the set of all Regions of
            Interest. If there is no current Region of Interest then nothing is
            done.
        */
        void add_current_to_set (void);
        
        
        /**
            Removes the specified Region of Interest from the set.
        */
        void remove_region (RoI*);
        
        
        /**
            Returns a vector of all the Regions of Interest.
        */
        vector<RoI> get_regions (void);
};

#endif
