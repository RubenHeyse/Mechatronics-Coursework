/* Test program for incremental encoder state machine code */
/* Serial input aspects are based closely upon: 
   http://forum.arduino.cc/index.php?topic=396450
   Example 4 - Receive a number as text and convert it to an int
   Modified to read a float */

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
boolean state = false;
void setup() {
    Serial.begin(115200);
    Serial.println("Enter state as \"on\" or \"off\" and hit return\n");
    pinMode(13,OUTPUT);
}

void loop() 
{
    recvWithEndMarker();
    if (newData)
    {
        if(strcmp(strlwr(receivedChars),"on")==0 || strcmp(strlwr(receivedChars),"on\r")==0)
        {
          digitalWrite(13,true);
          Serial.println("Switching LED on");
        }
        else if(strcmp(strlwr(receivedChars),"off")==0 || strcmp(strlwr(receivedChars),"off\r")==0)
        {
          digitalWrite(13,false);
          Serial.println("Switching LED off");
        }
        else
        {
           Serial.print("Unrecognised text: ");
           Serial.println(receivedChars);
           Serial.println(strlen(receivedChars));
        }
        newData=false;
    }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}


