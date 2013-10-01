
/*

Project:      Virtual sprayer Transmitter
Author:       Jason Lawley
Date:         9/15/13

Leonardo board

 
 */
 
#include <VirtualWire.h>



 // Constant pin connections
const int LB_TX       = 11;   // Light break output (38kHz)
const int LB_RX       = 7;    // Light break innput
const int LED_0       = 13;   // On-board LED
const int THRESHOLD   = 500;
int count             = 0;
int sensorValue       = 0;

// Virtual wire transmitter is pin 12


const int CTRL_0      = 8;    // Light break detected

int LBState = HIGH;           // Light break state

void setup(){
  //start serial connection
  Serial.begin(9600);
  // Configure input and output pins
  Serial.println("Start of setup");
  
  // Pushbutton inputs with internal pull-up resistor
  pinMode(LB_RX, INPUT);
  
  // Outputs
  pinMode(LED_0, OUTPUT); 
  pinMode(LB_TX, OUTPUT);     // Default value already set to 12 in library
  pinMode(CTRL_0, OUTPUT);

  //tone (LB_TX, 38000);
  
  // Setup for the virtual_wire library
  //vw_set_ptt_inverted(true);  // Required for DR3100
  digitalWrite(CTRL_0, false); 
  vw_setup(2000);             // Bits per sec
  //tone (LB_TX, 38000);

}



void loop(){
  
  Serial.println("Start of loop");
  
  
  const char *msg = "hello";

  // Once a break is detected, send the message 
  sensorValue = analogRead(A0);
  while (sensorValue > THRESHOLD) {
    sensorValue = analogRead(A0);
    Serial.println(sensorValue);
  }
  //noTone (LB_TX);
  
  // print out the value you read:
  Serial.println("Sensor Value:");
  Serial.println(sensorValue);
  count++;  
  Serial.println("Trigger #:");
  Serial.println(count);
  Serial.println();
  
  // Assert the CTRL signal to the light controller board
  digitalWrite(CTRL_0, true);
  
  digitalWrite(LED_0, true); // Flash a light to show transmitting
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(LED_0, false);
  delay(200);
  
  // Blink board LED 5 time when we either get a signal or the button
  // is pressed.
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_0, true);
    delay(50);
    digitalWrite(LED_0, false);
    delay(50);
  }

  // Deassert control signal to light board
  digitalWrite(CTRL_0, false); 
  sensorValue = analogRead(A0);
  
  
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//****                    FUNCTIONS                                        ****
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

// Turn off all of the valves
//void all_valves_off (){
//  digitalWrite (CTRL_0, LOW);
//  digitalWrite (CTRL_1, LOW);
//  digitalWrite (CTRL_2, LOW);
//}


