
/*

Project:      Virtual sprayer receiver
Author:       Jason Lawley
Date:         9/15/13

Description:  The following is code is to create a 'Virtual Sprayer Receiver'
or VSRX.  The VSRX, when paired with a Virtual Sprayer Transmitter (VSTX)
replaces a conventional dunk tank.  The person to be dunked sits in front of 
three sprayer heads.  The sprayer heads are controlled by three different valves 
that can be independently controlled.

There are two inputs to the VSRX - one is the a wireless receiver that will have 
will have data sent to it by the VSTX.  When the VSTX indicates a 'hit', 
the VSRX will cause the three valves to open can soak the dunkee.  The three independent 
valves are controlled to to create different displays of water or to 'dunk' the dunkee
with varying degrees of drenchedness.

A small little black button (LBB) located at the receiver box that when  
pressed will trigger the spraying to start.  This LBB is used for testing 
purposes or to manually trigger a spraying event.
 
 */
 

#include <VirtualWire.h>

 // Constant pin connections
const int LBB         = 4;    // Little black button
const int RX          = 2;
const int LED_0       = 13;   // On-board LED
const int CTRL_0      = 12;   // Valve control
const int CTRL_1      = 10;   // Valve control
const int CTRL_2      = 8;    // Valve control

int BRBState = HIGH;          // Variable for pushbuttons
int LBBState = HIGH;          // Variable for pushbuttons
int counter = 0;
int rand_select;
int buttonPush = LOW;


 
void setup(){
  //start serial connection
  Serial.begin(9600);
  // Configure input and output pins
  Serial.println("Start of setup");
  
  
  // Pushbutton inputs with internal pull-up resistor
  pinMode(LBB, INPUT_PULLUP);
  
  // Outputs
  pinMode(LED_0, OUTPUT); 
  pinMode(CTRL_0, OUTPUT);
  pinMode(CTRL_1, OUTPUT);
  pinMode(CTRL_2, OUTPUT);
  
  randomSeed(analogRead(A0)); //Seed the pseudo-random engine with some true randomness.
  
  // Setup for the virtual_wire library
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(2000);             // Bits per sec
  vw_set_rx_pin(RX);
  vw_rx_start();              // Start the receiver PLL running

  
  all_valves_on();
  delay(5000);
  all_valves_off();

}

void loop(){
  
  
  Serial.println("Start of loop");
  
  // Declare the buffers for the virtual wire
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  
  // Wait for a good message to be received via the wireless receiver 
  // or the LBB to be pressed.  When one of these happens, exit and start 
  // the spraying sequence.
  while (buttonPush == LOW) {
  
  if (vw_get_message(buf, &buflen)) { // Non-blocking
    
    int i;

    digitalWrite(LED_0, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
  
    for (i = 0; i < buflen; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
  
    Serial.println("");
        digitalWrite(LED_0, false);
        
    buttonPush = HIGH;
  }
  
  LBBState = digitalRead(LBB);
    if (!LBBState)
      buttonPush = HIGH;
  }
  
  // Blink board LED 5 time when we either get a signal or the button
  // is pressed.
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_0, true);
    delay(50);
    digitalWrite(LED_0, false);
    delay(50);
  }
  
  
  // Here we select one of 8 different patterns to implement
  rand_select = random(8);
  Serial.println("Random pattern selected is:");
  Serial.println(rand_select, DEC);
  
  switch (rand_select) {
    case 0:
      pattern_0();
      break;
    case 1:
      pattern_1();
      break;
    case 2:
      pattern_2();
      break;
    case 3:
      pattern_3();
      break;
    case 4:
      pattern_4();
      break;
    case 5:
      pattern_5();
      break;
    case 6:
      pattern_6();
      break;
    case 7:
      pattern_7();
      break;
  }
  
 
  // Turn everything off
  all_valves_off();
  buttonPush = LOW;

  Serial.println("Cycle completed");
  
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//****                    FUNCTIONS                                        ****
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

// Turn off all of the valves
void all_valves_off (){
  digitalWrite (CTRL_0, LOW);
  digitalWrite (CTRL_1, LOW);
  digitalWrite (CTRL_2, LOW);
}


// Turn on all of the valves
void all_valves_on (){
  digitalWrite (CTRL_0, HIGH);
  digitalWrite (CTRL_1, HIGH);
  digitalWrite (CTRL_2, HIGH);
}


// Turn on the valves from left to right
// Delay value for how long the valves are on/off
// Also can pass how many times there is a cycle
void cycle_LR (int delay_val, int times) {
  
  for (int i=0; i < times; i++) {
    digitalWrite (CTRL_0, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_0, LOW);
    delay (delay_val);
    digitalWrite (CTRL_1, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_1, LOW);
    delay (delay_val);
    digitalWrite (CTRL_2, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_2, LOW);
    delay (delay_val);
  }
}


// Turn on the valves from left to right
// Delay value for how long the valves are on/off
// Also can pass how many times there is a cycle
void cycle_RL (int delay_val, int times) {
  
  for (int i=0; i < times; i++) {
    digitalWrite (CTRL_2, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_2, LOW);
    delay (delay_val);
    digitalWrite (CTRL_1, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_1, LOW);
    delay (delay_val);
    digitalWrite (CTRL_0, HIGH);
    delay (delay_val);
    digitalWrite (CTRL_0, LOW);
    delay (delay_val);
  }
}

void outside_on (int delay_val) {
  digitalWrite (CTRL_0, HIGH);
  digitalWrite (CTRL_2, HIGH);
  delay (delay_val);
  digitalWrite (CTRL_0, LOW);
  digitalWrite (CTRL_2, LOW);
}

void middle_on (int delay_val) {
  digitalWrite (CTRL_1, HIGH);
  delay (delay_val);
  digitalWrite (CTRL_1, LOW);
}  

void left_two_on (int delay_val) {
  digitalWrite (CTRL_0, HIGH);
  digitalWrite (CTRL_1, HIGH);
  delay (delay_val);
  digitalWrite (CTRL_0, LOW);
  digitalWrite (CTRL_1, LOW);
}

void right_two_on (int delay_val) {
  digitalWrite (CTRL_1, HIGH);
  digitalWrite (CTRL_2, HIGH);
  delay (delay_val);
  digitalWrite (CTRL_1, LOW);
  digitalWrite (CTRL_2, LOW);
}

void rlm_cycle (int delay_val, int times) {
  
  digitalWrite (CTRL_1, HIGH);
  for (int i=0; i < times; i++) {
      digitalWrite (CTRL_2, HIGH);
      digitalWrite (CTRL_0, LOW);
      delay (delay_val);
      digitalWrite (CTRL_2, LOW);
      digitalWrite (CTRL_0, HIGH);
      delay (delay_val);
  }
  digitalWrite (CTRL_0, LOW);
  digitalWrite (CTRL_1, LOW);
  digitalWrite (CTRL_2, LOW);
  
}


  
void pattern_0() {
  
  Serial.println("Pattern 0");
  outside_on(300);
  delay(500);
  middle_on(500);
  delay(500);
  left_two_on(300);
  delay(500);
  right_two_on(300);
  delay(750);
  cycle_RL(300, 1);
  delay(300);
  cycle_LR(300, 1);
}

void pattern_1 () {

  Serial.println("Pattern 1");
  all_valves_on();
  delay (300);
   
  all_valves_off();
  delay (300);
  
  cycle_LR(300, 3);
  cycle_RL(300, 2);
  cycle_LR(300, 1);
}

void pattern_2() {
  
  Serial.println("Pattern 2");
  for (int i=0; i < 5; i++) {
    middle_on(500);
    delay(300);
    outside_on(500);
    delay(300);
  }
}

void pattern_3() {
  
  Serial.println("Pattern 3");
  cycle_LR(300, 2);
  delay(300);
  cycle_RL(300, 2);
  delay(500);
  middle_on(700);
  delay(400);
  middle_on(300);
  delay(200);
  middle_on(300);
}


void pattern_4() {
  
  Serial.println("Pattern 4");
  left_two_on(400);
  delay(300);
  right_two_on(400);
  delay(500);
  cycle_RL(300, 1);
  delay(200);
  cycle_LR(300, 1);
}


void pattern_5() {
  
  Serial.println("Pattern 5");
  middle_on(500);
  delay(500);
  right_two_on(400);
  delay(500);
  left_two_on(400);
  delay(500);
  cycle_RL(200, 2);
  delay(200);
  cycle_LR(200, 2);
}

void pattern_6() {
  
  Serial.println("Pattern 6");
  
  rlm_cycle(600, 3);
  delay(300);
  cycle_RL(300, 3);
  delay(500);
  cycle_LR(300, 3);
  delay(1000);
  middle_on(1000);
  delay(500);
  middle_on(150);
  delay(250);
  middle_on(150);
}


void pattern_7() {
  
  Serial.println("Pattern 7");
  
  left_two_on(500);
  delay (300);
  right_two_on(500);
  delay(500);
  rlm_cycle(500,4);
  delay(300);
  middle_on(500);
}
