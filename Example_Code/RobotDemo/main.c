#include <stdio.h>
#include <stdlib.h>
#include "Serial.h"
int main()
{
    char Buffer[50];
    sprintf (Buffer,"Hello World\n");
    SendViaSerial(Buffer);

    WaitForReply ();

    sprintf (Buffer,"Hello World (again)\n");
    SendViaSerial(Buffer);

    return 0;
}
