#include <errno.h>
extern int errno;

#define bdrate 115200               /* 115200 baud */
#define debugState 1

void Wakeup_Robot(char *buffer);
void SendCommands (char *buffer);
void Initialise_Robot (char *buffer);

#define grid_cell_width_mm  30
#define grid_cell_height_mm 30
#define grid_cell_x_count   3
#define grid_cell_y_count   3

typedef struct CELL{
    // 2 element array for coordinates, [x, y]
    int global_origin[2];
    int shape_local_origin[2];
} CELL;

#define scaled_shape_width_mm   20   
#define shape_point_width       16