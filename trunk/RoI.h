#ifndef _ROI_H
#define _ROI_H

#include <vector>
#include <string>

using namespace std;


struct coords {
    int x;
    int y;
};


class ROIEntity {
    public:
        vector<coords> points;
        string type;
};


class ROI {
    private:
        bool active;
        int colour_red;
        int colour_green;
        int colour_blue;
        string name;
        vector<ROIEntity> entities;
        
    public:
        ROI (void);
        
        ~ROI (void);
        
        void set_name (string);
        
        string get_name (void);
        
        void set_colour (int, int, int);
        void set_color (int, int, int);
        
        void get_colour (int*, int*, int*);
        void get_color (int*, int*, int*);
        
        void add_entity (ROIEntity);
        
        vector<ROIEntity> get_entities (void);
};


class ROISet {
    private:
        vector<ROI> regions;        // the set of all Regions of Interest
        ROI* current_region;        // the Region of Interest currently being worked on
        ROIEntity* current_entity;  // the shape currently being worked on
        bool editingEntity;         // true if currently creating an entity
        
        
    public:
        ROISet ();
        
        virtual ~ROISet (void);
        
        
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
            Returns true if an entity is currently being created.
        */
        bool editing ();
        
        
        /**
            Adds the shape that was just created to the Region of Interest.
        */
        void add_entity_to_current (void);
        
        
        /**
            Creates a new Region of Interest. If there is a region that is
            currently be worked on, it is replaced by the new one.
            Returns the newly created region.
        */
        ROI* new_region (void);
        
        /**
            Creates a new Region of Interest, with a specified name. If there is
            a region that is currently be worked on, it is replaced by the new
            one.
            Returns the newly created region.
        */
        ROI* new_region (string);
        
        
        /**
            Sets the Region of Interest to work on. The Region of Interest does
            not have to be in the set.
        */
        void set_current (ROI*);
        
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
        void remove_region (ROI*);
        
        /**
            Removes a Region of Interest from the set by matching the name.
        */
        void remove_region (string);
        
        
        /**
            Returns a vector of all the Regions of Interest.
        */
        vector<ROI> get_regions (void);
        
        
        /**
            Returns the number of Regions of Interest in the set.
        */
        int get_regions_count (void);
        
        
        /**
            Returns true if there is already a Regions of Interest in the set
            with the given name.
        */
        bool name_exists (string);
};

extern ROISet *regionsSet;

#endif
