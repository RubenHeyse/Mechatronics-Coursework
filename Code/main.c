#include <stdio.h>
#include <stdlib.h>
// #include <conio.h>
#include <curses.h>
// #include <windows.h>
#include "utilities/rs232.h"
#include "utilities/serial.h"
// #include "utilities/helperFunctions.h"
#include "main.h"

void Initialise_Cells(){

    CELL *cell_array;

    int cell_count = grid_cell_x_count*grid_cell_y_count;

    // call malloc to allocate that appropriate number of bytes for the array
    cell_array = (CELL *)malloc(sizeof(cell_count * sizeof(CELL))); 

    if(cell_array == NULL) {
      fprintf(stderr, "Memory Allocation Error: %s\n", strerror( errno ));    
    }

    int cell_index = 0;

    float shape_padding = (grid_cell_width_mm - scaled_shape_width_mm)/2;

    for (int x=0; x<grid_cell_x_count; x++){
        for (int y=0; y<grid_cell_y_count; y++){
            cell_array[cell_index].global_origin[0] = grid_cell_width_mm * x;
            cell_array[cell_index].global_origin[1] = grid_cell_height_mm * y;
            cell_array[cell_index].shape_local_origin[0] = grid_cell_width_mm * x + shape_padding;
            cell_array[cell_index].shape_local_origin[1] = grid_cell_height_mm * y + shape_padding;
            cell_index++;

            #ifdef debugState
                printf("Global coordinates [%d, %d]", 
                    cell_array[cell_index].global_origin[0], 
                    cell_array[cell_index].global_origin[1]
                );

                printf("Shape local coordinates [%d, %d]", 
                    cell_array[cell_index].shape_local_origin[0], 
                    cell_array[cell_index].shape_local_origin[1]
                );
            #endif
        }
    }
}

int main()
{
    char buffer[100];
    
    // If we cannot open the port then give up immediatly
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    Wakeup_Robot(&buffer[0]);

    Initialise_Robot(&buffer[0]);

    Initialise_Cells();

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
    sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}

//These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands

void InitialiseRobot (char *buffer){
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);
}

void Wakeup_Robot(char *buffer){
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
}