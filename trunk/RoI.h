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
        int color_red;
        int color_green;
        int color_blue;
        string name;
        vector<RoIEntity*> entities;
        
    public:
        RoI (void);
        
        ~RoI (void);
        
        void set_name (string);
        
        string get_name (void);
        
        void set_color (int, int, int);
        
        void get_color (int*, int*, int*);
        
        void add_entity (RoIEntity*);
        
        vector<RoIEntity*> get_entities (void);
};


class RoISet {
    private:
        vector<RoI*> regions;
        RoI* current_region;
        RoIEntity* new_entity;
        
    public:
        RoISet (void);
        
        ~RoISet (void);
        
        
        /**
            Creates a new Region of Interest. If an old region exists, it is
            deleted.
        */
        void new_region (void);
        
        /**
            Creates a new Region of Interest, with a specified name. If an old
            region exists, it is deleted.
        */
        void new_region (string);
        
        
        void start_new (string);
        
        void add_point (int, int);
        
        void backtrack (void);
        
        void add_new_to_current (void);
        
        vector<RoI*> get_regions (void);
        
        void set_current (RoI*);
        
        void set_current (string);
        
        void delete_region (RoI*);
};

#endif
