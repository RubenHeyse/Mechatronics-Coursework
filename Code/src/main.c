#include "../include/main.h"

int main()
{
    char buffer[100];
    int res = 0;
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

    char shape_file_path[255] = "/home/ruben/github/Mechatronics-Coursework/Code/shape-files/ShapeStrokeData.txt";

    res = 0;
    do{
        res = parse_shape_file(shape_file_path, shape_array);
    } while (!res);
    
    // These are sample commands to draw out some information - these are the ones you will be generating.
    sprintf (buffer, "G0 X-13.41849 Y0.000\n");
    SendCommands(buffer);
    sprintf (buffer, "S1000\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41849 Y-4.28041\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41849 Y0.0000\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41089 Y4.28041\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);
    sprintf (buffer, "G0 X-7.17524 Y0\n");
    SendCommands(buffer);
    sprintf (buffer, "S1000\n");
    SendCommands(buffer);
    sprintf (buffer, "G0 X0 Y0\n");
    SendCommands(buffer);
    

    sleep(2000);

    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    return (0);
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands (char *buffer )
{
    // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    WaitForReply();
    // sleep(100); // Can omit this when using the writing robot but has minimal effect
    getch(); // Omit this once basic testing with emulator has taken place
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

    #ifdef debugState
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

            #ifdef debugState
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

int parse_shape_file(char shape_file_path[255], SHAPE *shape_array){

    FILE *shape_file_data;
    
    shape_file_data = fopen(shape_file_path, "r");

    #ifdef debugState
        printf("File opened from: %s\n", shape_file_path);
    #endif

    if (shape_file_data == NULL)
    {
        printf("Error: could not open file %s\n", shape_file_path);
        return 0;
    }

    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 30;
    char line_buffer[MAX_LENGTH];

    int first_line = true;
    int shape_count = 0;
    char shape_name[20];
    int current_shape = 0;
    int instruction_line_count = 0;

    while (fgets(line_buffer, MAX_LENGTH, shape_file_data)) {

        #ifdef debugState
            printf("line buffer: %s", line_buffer);
        #endif

        //Extract number of shapes from first line
        if (first_line){
            sscanf(line_buffer, "%*s %d", &shape_count);
            // call malloc to allocate that appropriate number of bytes for the array
            shape_array = (SHAPE *)calloc(shape_count, sizeof(SHAPE)); 

            #ifdef debugState
                printf("shape count: %d\n", shape_count);
            #endif

            if(shape_array == NULL) {
                fprintf(stderr, "Shape Array -> Memory Allocation Error: %s\n", strerror( errno )); 
                return 0;   
            }

            first_line = false;

        } else {

            if (isalpha(line_buffer[0])){
                sscanf(line_buffer, "%19s %d", shape_name, &instruction_line_count);
                strcpy(shape_array[current_shape].name, shape_name);
                current_shape++;

                #ifdef debugState
                    printf("shape name: %s\n", shape_name);
                    printf("instrucion line count: %d\n", instruction_line_count);
                #endif
            }
            
            
            
        }
    }


        define_shapes();

    // close the file
    fclose(shape_file_data);

    return 1;
}

int define_shapes(){
    return 1;
}