#ifndef _ROISET_H
#define _ROISET_H

#include "ROI.h"

class ROISet {
    private:
        std::vector<ROI*> regions;       // the set of all Regions of Interest
        ROI* current_region;        // the Region of Interest currently being worked on
        ROIEntity* current_entity;  // the shape currently being worked on
        bool editingEntity;         // true if currently creating an entity
        bool unsavedChanges;        // true if there have been changes made after the last save
        
        
    public:
        /**
            Creates an empty set of Regions of Interest.
        */
        ROISet ();
        
        /**
            Default destructor - does nothing.
        */
        virtual ~ROISet (void);
        
        
/* Entities *******************************************************************/
        
        /**
            Creates a new shape that will be part of the Region of Interest. The
            parameter passed in specifies what type of shape it will be - POINT,
            RECT, POLY.
        */
        void new_entity (const char*);
        
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
            Adds the shape that was just created to the Region of Interest and
            prevents further editing of it.
            Prevents incorrect entities from being added, so number of points
            must be correct for the type specified.
        */
        void finish_entity (void);
        
        /**
            Allows the user to specify if they want to add the shape that was
            just created to the Region of Interest, then stops editing it.
            Prevents incorrect entities from being added, so number of points
            must be correct for the type specified.
        */
        void finish_entity (bool);
        
        
        /**
            Returns true if an entity is currently being created.
        */
        bool editing ();
        
        /**
            Returns the type of entity that is currently being created.
            ROI_NONE is returned if no entity is being created at the moment.
        */
        const char* editingType ();
        
        
        ROIEntity* get_current_entity();        
        
        
/* Regions Of Interest ********************************************************/
        
        /**
            Creates a new Region of Interest with a unique name. If there is a
            region that is currently be worked on, it is replaced by the new
            one.
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
            Sets the Region of Interest to work on, based on its name. The
            region must already exist in the set otherwise nothing happens.
            Returns the current Region of Interest that has just been set. If
            a new current was not set then NULL is returned.
        */
        ROI* set_current (string);
        
        
/* Set ************************************************************************/
        
        /**
            Adds the specified Region of Interest to the set. If the second
            parameter is set to true then if an existing Region of Interest has
            the same name it is replaced by the new one. If it is false then
            nothing happens.
        */
        void add_region_to_set (ROI*, bool);
        
        
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
        std::vector<ROI*> get_regions (void);
        
        
        /**
            Returns the number of Regions of Interest in the set.
        */
        int get_regions_count (void);
        
        
        /**
            Returns true if there is already a Regions of Interest in the set
            with the given name.
        */
        bool name_exists (string);
        
        
        /**
            Removes all the ROIs that are in the set. This can be used to empty
            the set when a new one is loaded or to clear the set when a new
            image is loaded.
        */
        void delete_all_regions ();
        
        /**
            Combines the ROIs from the specified set with the current set. If
            the second parameter is true then any ROIs that have the same name
            in the current set will be replaced by versions from the new set.
        */
        void combine (ROISet*, bool);
        
        
        /**
            This is called to tell the set that the Regions of Interest have
            been saved.
        */
        void save_done () { unsavedChanges = false; }
        
        /**
            Returns true if there have been changes made since the set was last
            saved. This is used to prompt the user if they are going to lose
            any unsaved Regions of Interest.
        */
        bool unsaved_changes () { return unsavedChanges; }
};


extern ROISet *regionsSet;

#endif
