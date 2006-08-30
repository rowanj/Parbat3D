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

#endif
