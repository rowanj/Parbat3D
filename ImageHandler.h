#include <Windows.h>

#include "ImageProperties.h"

typedef struct pixel_values_t {
        char number_bands;
        int[] value;
} pixel_values, *pixel_values_ptr;

typedef struct geo_coords_t {
        int x;
        int y;
        float lattitude;
        float longitude;
} geo_coords, *geo_coords_ptr;

class ImageHandler
{
public:
       ImageHandler(HWND overview_hwnd, HWND main_hwnd, char* filename, int* status_enum);
       virtual ~ImageHandler(void);
       void redraw(void);
       ImageProperties* get_image_properties(void);
       void resize_window(void);
       PRECT get_viewport(void);
       PRECT set_viewport(void);
       pixel_values_ptr get_pixel_values(unsigned int x, unsigned int y);
       void get_geo_pos(geo_coords_ptr pos);
}
      
