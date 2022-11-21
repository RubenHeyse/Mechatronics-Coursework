#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//#define PC_MODE

#ifdef PC_MODE
void SendViaSerial(char *Buffer)
{
    printf ("The command to send is %s", Buffer);
}

void WaitForReply (void)
{
    char c;
    c = getch();
    return;
}

#else

void SendViaSerial(char *Buffer)
{
    printf ("R: The command to send is %s", Buffer);
}

void WaitForReply (void)
{
    char c;
    c = getch();
    return;
}

#endif // PC_MODE


