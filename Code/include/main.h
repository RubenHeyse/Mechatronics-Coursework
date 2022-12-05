#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "../include/rs232.h"
#include "../include/serial.h"

extern int errno;

#define bdrate 115200               /* 115200 baud */
#define debugState

#define grid_cell_width_mm  30
#define grid_cell_height_mm 30
#define grid_cell_x_count   3
#define grid_cell_y_count   3

typedef struct CELL{
    // 2 element array for coordinates, [x, y]
    int global_origin[2];
    int shape_local_origin[2];
} CELL;

int wakeup_robot(char *buffer);
void SendCommands(char *buffer);
int initialise_robot(char *buffer);
int initialise_cells(CELL *cell_array);

typedef struct SHAPE{
    char name[20];
    int instructions[10][10][10];
} SHAPE;

#define scaled_shape_width_mm   20   
#define shape_point_width       16

int parse_shape_file(char *shape_file_path, SHAPE *shape_array);
int define_shapes();
