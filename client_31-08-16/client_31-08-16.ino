/*
  DNS and DHCP-based Web client
 
 This sketch connects to a website (http://talne.mtacloud.co.il/)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 2 Sep 2016
 by Assaf Dekel

*/

// Gas Station Constants
#define LED1 2
#define LED2 3
#define LED3 4
#define LED4 5
#define LED5 6
#define LED6 7


#define REFRESH_TIME 3000
#define PUMP_ON_TIME 5000

// ---------------------

// php website constants
// Command string structure is
// #newCmd=True,pump=3#
#define NEW_CMD_PHRASE "newCmd"
#define PUMP_PHRASE "pump"

// HTTP request
// Structure is:
// GET /arduino.php
// Host: talne.mtacloud.co.il

#define WEBSITE "talne.mtacloud.co.il"
#define PORT 80
//#define HTTP_GET_BEGIN "GET /arduino.php?"
#define HTTP_GET_BEGIN "GET /arduino.php"
#define HTTP11 "HTTP/1.1"
#define HOST_HEADER "Host: "

#define REQUEST_PHRASE "cmd"
#define REQUEST_POLL "poll" // poll for new command

#define LED1_WEB_INDEX "1"
#define LED2_WEB_INDEX "2"
#define LED3_WEB_INDEX "3"
#define LED4_WEB_INDEX "4"
#define LED5_WEB_INDEX "5"
#define LED6_WEB_INDEX "6"

// -------------------------

#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xA0, 0xAA, 0xDB, 0x0F, 0x8E, 0x12 };
char serverName[] = WEBSITE;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
boolean newCommandFlag = false;

void setup() {
  // initialize the digital switching pins as an output.
  // (Initializing LED pins)
  
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);

  // LEDs Check
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  digitalWrite(LED3,HIGH);
  digitalWrite(LED4,HIGH);  
  digitalWrite(LED5,HIGH);
  digitalWrite(LED6,HIGH);

  delay(1000);
  
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);  
  digitalWrite(LED5,LOW);
  digitalWrite(LED6,LOW);
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("-------------------------- SETUP FUNCTION --------------------------");
  Serial.println("Serial on!");
  
      // DBG
    Serial.print("Database is on:           ");
    Serial.println(WEBSITE);
    Serial.print("Refresh time (in ms) is:  ");
    Serial.println(REFRESH_TIME);
    Serial.println();

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  Serial.println("Waiting a sec for Ethernet...");
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("Setup is done!  Let's get started!");
}


// ----------------------------- Functions -------------------------

// Sends poll http GET request to WEBPAGE using defined constants
void sendHttpPollRequest(EthernetClient client)
{
                Serial.println();
                Serial.println("Sending HTTP GET request:");
                client.print(HTTP_GET_BEGIN);
                //client.print(REQUEST_PHRASE);
                //client.print("=");
                //client.print(REQUEST_POLL);
                client.print(" ");
                client.println(HTTP11);
                
                client.print(HOST_HEADER);
                client.println(WEBSITE);
                client.println();
                
                // DBG - Print HTTP request to Serial
                Serial.print(HTTP_GET_BEGIN);
                //Serial.print(REQUEST_PHRASE);
                //Serial.print("=");
                //Serial.print(REQUEST_POLL);
                Serial.print(" ");
                Serial.println(HTTP11);
                
                Serial.print(HOST_HEADER);
                Serial.println(WEBSITE);
                Serial.println();
                Serial.println();
                
                return;         
}

String readCmdStr(EthernetClient client)
{// returns the whole command string from website
    
        Serial.println("-- entered readCmdSrt function --");
        String str = "";
        // if there are incoming bytes available
        if (client.available())
        {
            // structure is ###payload###
            char c = client.read();
            // skip all chars before '#'
            while (c != '#')
            {
                c = client.read();
            }
            
            // read command string between #..#
            c = client.read();
            
            while (c != '#' and client.available())
            {
                str.concat(c);
                c = client.read();
                
                // if done reading command string
                if (c == '#')
                {
                   return str;
                }       
            }
            // if done before reaching the ending #
            Serial.println("return empty string");
            Serial.println("--- END OF function: readCmdStr ---");
            return "";
        }
        else
        {
          Serial.println("-- ERROR: readCmdSrt function - client.available() returned FALSE --");
        }
}


String findInfo(String cmdStr,String infoPhrase)
{ // INPUT: a full 'command string' cmdStr
  // INPUT: a phrase to look for in the 'command string'
  // OUTPUT: the expresion in 'cmdStr' that follows the '=' sign after 'infoPhrase'
  // example:
  // INPUT:
  // cmdStr = #newCmd=True,cmdContent=on,cmdCount=57,dateIssued=12.01.13,timeIssued=14:30#
  // infoPhrase = newCmd
  // OUTPUT = True
    
    String info = "";

    // find the relevant phrase in command string
    int phraseIndx = cmdStr.indexOf(infoPhrase);
    // find index of the first character of the relevant info
    int infoIndx = cmdStr.indexOf('=',phraseIndx) + 1;
    
    // read info char by char until reaching ',' or '#'
    char c = cmdStr.charAt(infoIndx);
    while ((c != ',') and (c!=0))
    {
      info.concat(c);
      c = cmdStr.charAt(++infoIndx);
    }
    
    return info;
}

// ----------------------------- LOOP MAIN FUNCTION -------------------------


void loop()
{
  String tmpStr;
  String cmdStr;
  
  // Declare where we are and what we are doing:
  Serial.println();
  Serial.println("-------------------------- LOOP FUNCTION --------------------------");
  Serial.print("Waiting ");
  Serial.print(REFRESH_TIME);
  Serial.println("ms to refresh");

  delay(REFRESH_TIME);
  
  Serial.println("Checking for a new command:");
  
  Serial.println();
  Serial.println();
  
  // Connecting to WEBSITE
  Serial.println("Connecting to server...");
  if (client.connect(WEBSITE, PORT))
  {   // Connected to server!
      Serial.println("Connected to server!");
      
      // Send http get request to php server (WEBSITE)
      sendHttpPollRequest(client);
      
      // Wait a bit for a response
      delay(1000);
      if (!client.available()) {
        delay(3000);
      }
      
      // Read server's response into a string
      cmdStr = readCmdStr(client);
      Serial.println("Command string:");
      Serial.println(cmdStr);
      
      // Disconnect from server
      client.stop();
      
      // Check in server's response if we got a new command
      tmpStr = findInfo(cmdStr,NEW_CMD_PHRASE);
      Serial.println();

      // Update the 'New Command Flag'
      newCommandFlag = tmpStr.equalsIgnoreCase("True");
      
  }else
  {   // Connection to server Failed
      Serial.println("Connection to server Failed!");
      client.stop();
      newCommandFlag = false;
  }
  
  // If we got a new command:
  if (newCommandFlag)
  {   // New command received
      newCommandFlag = false;
      Serial.println("HEY! I got a new command!");
      
      // Check which pump to open
      tmpStr = findInfo(cmdStr,PUMP_PHRASE);

      if (tmpStr.equalsIgnoreCase(LED1_WEB_INDEX))
      {
        // open pump 1
        Serial.println("$$$$$$$$$ LED1 - TURN ON! $$$$$$$$$");
        digitalWrite(LED1,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED1,LOW);
      }
      
      if (tmpStr.equalsIgnoreCase(LED2_WEB_INDEX))
      {
        // open pump 2
        Serial.println("$$$$$$$$$ LED2 - TURN ON! $$$$$$$$$");
        digitalWrite(LED2,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED2,LOW);
      }
      
      if (tmpStr.equalsIgnoreCase(LED3_WEB_INDEX))
      {
        // open pump 3
        Serial.println("$$$$$$$$$ LED3 - TURN ON! $$$$$$$$$");
        digitalWrite(LED3,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED3,LOW);
      }
      
      if (tmpStr.equalsIgnoreCase(LED4_WEB_INDEX))
      {
        // open pump 4
        Serial.println("$$$$$$$$$ LED4 - TURN ON! $$$$$$$$$");
        digitalWrite(LED4,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED4,LOW);
      }
      
      if (tmpStr.equalsIgnoreCase(LED5_WEB_INDEX))
      {
        // open pump 5
        Serial.println("$$$$$$$$$ LED5 - TURN ON! $$$$$$$$$");
        digitalWrite(LED5,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED5,LOW);
      }
      
      if (tmpStr.equalsIgnoreCase(LED6_WEB_INDEX))
      {
        // open pump 6
        Serial.println("$$$$$$$$$ LED6 - TURN ON! $$$$$$$$$");
        digitalWrite(LED6,HIGH);
        delay(PUMP_ON_TIME);
        digitalWrite(LED6,LOW);
      }
  }else
  {
      Serial.println("No new command detected");  
  }
}

