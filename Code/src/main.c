#include "../include/main.h"

int main()
{
    char buffer[100];
    int res = 0,
        shape_count = 0;
    int *p_shape_count = &shape_count;
    CELL *cell_array;
    SHAPE *shape_array;
    
    // If we cannot open the port then give up immediatly
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    // res = 0;
    // do {
    //     res = wakeup_robot(&buffer[0]);
    // } while (!res);

    // res = 0;
    // do{
    //     initialise_robot(&buffer[0]);
    // } while(!res);

    res = 0;
    do{
        res = initialise_cells(cell_array);
    } while (!res);

    char shape_file_path[250] = "/home/ruben/github/Mechatronics-Coursework/Code/shape-files/ShapeStrokeData.txt";

    res = 0;
    do{
        res = parse_shape_file(shape_file_path, shape_array, p_shape_count);
    } while (!res);

    printf("shape name: %s", shape_array[0].name);
    printf("shape name: %s", shape_array[1].name);

    /*********************
     * 
     * Generate & send G-Code
     * 
    **********************/

    #ifdef DEBUG_MODE 
        printf("\n\n--------------GENERATING GCODE--------------\n\n");
    #endif

    FILE *drawing_file_data;
    
    char drawing_file_path[250] = "/home/ruben/github/Mechatronics-Coursework/Code/shape-files/DrawShapes.txt";

    drawing_file_data = fopen(drawing_file_path, "r");

    #ifdef DEBUG_MODE
        printf("File opened from: %s\n", drawing_file_path);
    #endif

    if (drawing_file_data == NULL)
    {
        printf("Error: could not open file %s\n", drawing_file_path);
        return 0;
    }

    const unsigned MAX_LENGTH = 255;
    char    line_buffer[MAX_LENGTH],
            shape_to_draw[MAX_LENGTH];

    int x_coord,
        y_coord,
        shape_index = 0,
        cell_index,
        shape_scale_factor = scaled_shape_width_mm/shape_point_width;

    float   x_move_pos,
            y_move_pos;

    bool    shape_found = false,
            pen_up = true;

    printf("shape name: %s", shape_array[0].name);
    

    // while (fgets(line_buffer, MAX_LENGTH, drawing_file_data)) {

    //     #ifdef DEBUG_MODE
    //         printf("line buffer: %s", line_buffer);
    //         printf("comparison: %p\n", strstr(line_buffer, "DRAW_GRID"));
    //     #endif


    //     if (strstr(line_buffer, "DRAW_GRID")){
    //         /*******
    //          * Draw grid
    //         *******/
    //         for (int x=1; x<grid_cell_x_count; x++){
    //             sprintf(buffer, "G1 X%d Y0\n", grid_cell_width_mm*x);
    //             SendCommands(buffer);
    //             sprintf(buffer, "S1000\n");
    //             SendCommands(buffer);
    //             sprintf(buffer, "G1 X%d Y%d\n", grid_cell_width_mm*x, grid_cell_height_mm*grid_cell_y_count);
    //             SendCommands(buffer);
    //             sprintf(buffer, "S0\n");
    //             SendCommands(buffer);
    //         }
    //         for (int y=1; y<grid_cell_y_count; y++){
    //             sprintf(buffer, "G1 X0 Y%d\n", grid_cell_height_mm*y);
    //             SendCommands(buffer);
    //             sprintf(buffer, "S1000\n");
    //             SendCommands(buffer);
    //             sprintf(buffer, "G1 X%d Y%d\n", grid_cell_height_mm*grid_cell_x_count, grid_cell_height_mm*y);
    //             SendCommands(buffer);
    //             sprintf(buffer, "S0\n");
    //             SendCommands(buffer);
    //         }

    //         #ifdef DEBUG_MODE
    //             printf("grid drawn\n");
    //         #endif

    //     } else {
    //         sscanf(line_buffer, "%d %d %255s", &x_coord, &y_coord, shape_to_draw);

    //         #ifdef DEBUG_MODE
    //             printf("x: %d, y: %d, shape: %s\n", x_coord, y_coord, shape_to_draw);
    //         #endif

    //         shape_found = false;
    //         #ifdef DEBUG_MODE
    //             printf("finding shape\n");
    //         #endif
    //         do{
    //             #ifdef DEBUG_MODE
    //                 printf("shape index: %d\n", shape_index);
    //                 printf("shape name: %s\n", shape_array[shape_index].name);
    //                 printf("shape name comparison: %p\n", strstr(shape_to_draw, shape_array[shape_index].name));
    //             #endif

    //             // if (strstr(shape_to_draw, shape_array[shape_index].name)){
    //             //     shape_found=true;

    //             //     #ifdef DEBUG_MODE
    //             //         printf("shape found at: %d\n", shape_index);
    //             //     #endif
    //             // }
    //             shape_index++;
    //         }while(shape_found == false);

    //         cell_index = (x_coord * y_coord)-1;

    //         #ifdef DEBUG_MODE
    //             printf("cell index: %d\n", cell_index);
    //         #endif

    //         // Move to shape local origin for requested cell
    //         #ifdef DEBUG_MODE
    //             printf ("Moving to shape local origin X%d Y%d\n", 
    //                 cell_array[cell_index].shape_local_origin[0], 
    //                 cell_array[cell_index].shape_local_origin[1]
    //             );
    //         #endif

    //         sprintf (buffer, "S0\n");
    //         SendCommands(buffer);
    //         sprintf (buffer, "G0 X%d Y%d\n", 
    //             cell_array[cell_index].shape_local_origin[0], 
    //             cell_array[cell_index].shape_local_origin[1]
    //         );
    //         SendCommands(buffer);

    //         for (int n=0; n<shape_array[shape_index].instruction_count; n++){
    //             #ifdef DEBUG_MODE
    //                 printf("instruction %d of %d\n", n, shape_array[shape_index].instruction_count);
    //                 printf("pen state: %i\n", pen_up);
    //             #endif

    //             if (shape_array[shape_index].instructions[n][2] == 1 && pen_up == true){
    //                 sprintf (buffer, "S1000\n");
    //                 SendCommands(buffer);
    //                 pen_up = false;

    //                 #ifdef DEBUG_MODE
    //                     printf("pen down\n");
    //                 #endif

    //             } else if (shape_array[shape_index].instructions[n][2] == 0 && pen_up == false){
    //                 sprintf (buffer, "S0\n");
    //                 SendCommands(buffer);

    //                 #ifdef DEBUG_MODE
    //                     printf("pen up\n");
    //                 #endif
    //             }

    //             x_move_pos = cell_array[cell_index].shape_local_origin[0] + shape_array[shape_index].instructions[n][0]*shape_scale_factor;
    //             y_move_pos = cell_array[cell_index].shape_local_origin[1] + shape_array[shape_index].instructions[n][1]*shape_scale_factor;

    //             #ifdef DEBUG_MODE
    //                 printf("local x: %d\n", cell_array[cell_index].shape_local_origin[0]);
    //                 printf("local y: %d\n", cell_array[cell_index].shape_local_origin[1]);
    //                 printf("moving to (%f, %f)\n", x_move_pos, y_move_pos);
    //                 printf("scale factor: %d\n", shape_scale_factor);
    //                 printf("x point: %d\n", shape_array[shape_index].instructions[n][0]);
    //                 printf("y point: %d\n", shape_array[shape_index].instructions[n][1]);
    //                 printf("x distance: %d\n", shape_array[shape_index].instructions[n][0]*shape_scale_factor);
    //                 printf("y distance: %d\n\n\n\n", shape_array[shape_index].instructions[n][1]*shape_scale_factor);
    //             #endif

    //             sprintf (buffer, "G0 X%f Y%f\n", x_move_pos, y_move_pos);
    //             SendCommands(buffer);

    //         }
    //     }
    // }

    #ifdef DEBUG_MODE
        printf("finished generating gcode\n");
    #endif
    
    sleep(2000);

    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    return (0);
}

// Send the data to the robot
void SendCommands (char *buffer )
{
    #ifdef DEBUG_MODE

        printf ("Buffer to send: %s", buffer);
        
        char *filename = "gcode.txt";

        // open the file for writing
        FILE *fp = fopen(filename, "a");
        
        // write to the text file
        for (int i = 0; i < 10; i++)
            fprintf(fp, "%s", buffer);

        printf("written to text file\n");
        // close the file
        fclose(fp);

    #elif
        PrintBuffer (&buffer[0]);
        WaitForReply();
        // sleep(100); // Can omit this when using the writing robot but has minimal effect
        getch(); // Omit this once basic testing with emulator has taken place
    #endif

}

//These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands

int initialise_robot(char *buffer){
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);

    return 1;
}

int wakeup_robot(char *buffer){
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");

    #ifdef DEBUG_MODE
        printf ("Buffer to send: %s", buffer);
    #endif

    PrintBuffer(&buffer[0]);
    
    sleep(100);

    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();

    printf ("\nThe robot is now ready to draw\n");

    return 1;
}

int initialise_cells(CELL *cell_array){

    int cell_count = grid_cell_x_count*grid_cell_y_count;

    // call malloc to allocate that appropriate number of bytes for the array
    cell_array = (CELL *)calloc(cell_count, sizeof(CELL)); 

    if(cell_array == NULL) {
      fprintf(stderr, "Cell Array -> Memory Allocation Error: %s\n", strerror( errno )); 
      return 0;   
    }

    int cell_index = 0;
    int cell_x_pos = 0;
    int cell_y_pos = 0;
    int shape_x_pos = 0;
    int shape_y_pos = 0;

    float shape_padding = (grid_cell_width_mm - scaled_shape_width_mm)/2;

    for (int x=0; x<grid_cell_x_count; x++){
        for (int y=0; y<grid_cell_y_count; y++){
            cell_x_pos = grid_cell_width_mm * x;
            cell_y_pos = grid_cell_height_mm * y;
            shape_x_pos = grid_cell_width_mm * x + shape_padding;
            shape_y_pos = grid_cell_height_mm * y + shape_padding;
            

            memcpy(&cell_array[cell_index].global_origin[0], &cell_x_pos, sizeof(int));
            memcpy(&cell_array[cell_index].global_origin[1], &cell_y_pos, sizeof(int));
            memcpy(&cell_array[cell_index].shape_local_origin[0], &shape_x_pos, sizeof(int));
            memcpy(&cell_array[cell_index].shape_local_origin[1], &shape_y_pos, sizeof(int));

            #ifdef DEBUG_MODE
                printf("---------CELL %d---------\n", cell_index);
                printf("x = %d\n", x);
                printf("x pos: %d\n", cell_x_pos);
                printf("y = %d\n", y);
                printf("y pos: %d\n", cell_y_pos);
                printf("shape x pos: %d\n", shape_x_pos);
                printf("shape y pos: %d\n", shape_y_pos);

                printf("Global coordinates: [%d, %d]\n", 
                    cell_array[cell_index].global_origin[0], 
                    cell_array[cell_index].global_origin[1]
                );

                printf("Shape local coordinates: [%d, %d]\n\n", 
                    cell_array[cell_index].shape_local_origin[0], 
                    cell_array[cell_index].shape_local_origin[1]
                );
                
            #endif

            cell_index++;
        }
    }

    return 1;
}

int parse_shape_file(char shape_file_path[255], SHAPE *shape_array, int *p_shape_count){

    FILE *shape_file_data;
    
    shape_file_data = fopen(shape_file_path, "r");

    #ifdef DEBUG_MODE
        printf("File opened from: %s\n", shape_file_path);
    #endif

    if (shape_file_data == NULL)
    {
        printf("Error: could not open file %s\n", shape_file_path);
        return 0;
    }

    // reading line by line, max 30 bytes
    const unsigned MAX_LENGTH = 30;
    char line_buffer[MAX_LENGTH];

    bool first_line = true;
    int current_shape = -1,
        instruction_line_count = 0,
        instruction_line_number = 0,
        instruction_1, instruction_2, instruction_3;

    char shape_name[30];

    while (fgets(line_buffer, MAX_LENGTH, shape_file_data)) {

        #ifdef DEBUG_MODE
            printf("\n----------\nline buffer: %s", line_buffer);
        #endif 

        //Extract number of shapes from first line
        if (first_line){
            sscanf(line_buffer, "%*s %d", p_shape_count);

            // call malloc to allocate that appropriate number of bytes for the array
            shape_array = (SHAPE *)calloc(*p_shape_count, sizeof(SHAPE)); 

            #ifdef DEBUG_MODE
                printf("shape count: %d\n", *p_shape_count);
            #endif

            if(shape_array == NULL) {
                fprintf(stderr, "Shape Array -> Memory Allocation Error: %s\n", strerror( errno )); 
                return 0;   
            }

            first_line = false;

        } else if (isalpha(line_buffer[0])){
                sscanf(line_buffer, "%s %d", shape_name, &instruction_line_count);
                strcpy(shape_array[current_shape+1].name, shape_name);
                memcpy(&shape_array[current_shape+1].instruction_count, &instruction_line_count, sizeof(int));
     

                #ifdef DEBUG_MODE
                    printf("current shape: %d\n", current_shape+1);
                    printf("\nshape name: %s\n", shape_name);
                    printf("copied shape name: %s\n", shape_array[current_shape+1].name);
                    printf("instruction line count: %d\n\n", instruction_line_count);
                #endif

                current_shape++;
                instruction_line_number = 0; 

        } else if (isdigit(line_buffer[0]) && instruction_line_number == 50){

            sscanf(line_buffer, "%d %d %d", &instruction_1, &instruction_2, &instruction_3);

            shape_array[current_shape].instructions[instruction_line_number][0] = instruction_1; 
            shape_array[current_shape].instructions[instruction_line_number][1] = instruction_2;
            shape_array[current_shape].instructions[instruction_line_number][2] = instruction_3;

            #ifdef DEBUG_MODE
                printf("\ncurrent shape: %d\n", current_shape);
                printf("instructions for line %d: %d %d %d \n",
                    instruction_line_number, 
                    shape_array[current_shape].instructions[instruction_line_number][0],
                    shape_array[current_shape].instructions[instruction_line_number][1],
                    shape_array[current_shape].instructions[instruction_line_number][2]
                );         
            #endif

            instruction_line_number++;

        }

        printf("looped\n");
    }

    // close the file
    fclose(shape_file_data);

    return 1;
}