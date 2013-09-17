
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
#include "LPD8806.h"
#include "SPI.h"


 // Constant pin connections
const int LB_TX       = 12;   // Light break output (38kHz)
const int LB_RX       = 10;   // Light break innput
const int LED_0       = 13;   // On-board LED

const int LT_R_DATA   = 16;   // Data for light pin
const int LT_L_CLK    = 15;   // Clock for light pin

//const int CTRL_0      = 12;   // Valve control
//const int CTRL_1      = 10;   // Valve control
//const int CTRL_2      = 8;    // Valve control

int LBState = HIGH;           // Light break state

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
LPD8806 strip = LPD8806(64, LT_R_DATA, LT_L_CLK);
 
void setup(){
  //start serial connection
  Serial.begin(9600);
  // Configure input and output pins
  Serial.println("Start of setup");
  
  
  // Pushbutton inputs with internal pull-up resistor
  pinMode(LB_RX, INPUT);
  
  // Outputs
  pinMode(LED_0, OUTPUT); 
//  pinMode(LB_TX, OUTPUT);     // Default value already set to 12 in library
//  pinMode(CTRL_1, OUTPUT);
//  pinMode(CTRL_2, OUTPUT);

  tone (LB_TX, 38000);
  
//  randomSeed(analogRead(A0)); //Seed the pseudo-random engine with some true randomness.
  
  // Setup for the virtual_wire library
  //vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(2000);             // Bits per sec

  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();


// Put a light test sequence here  

}

// function prototypes, do not remove these!
void colorChase(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void dither(uint32_t c, uint8_t wait);
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
void wave(uint32_t c, int cycles, uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(uint16_t WheelPos);

void loop(){
  
  Serial.println("Start of loop");
  
  const char *msg = "hello";

  while (!digitalRead(LB_RX)) {
    // Do nothing
  }
  
  // Once a break is detected, send the message 
  digitalWrite(13, true); // Flash a light to show transmitting
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(13, false);
  delay(200);
  
  // Once the message is sent, do some light magic
  // Send a simple pixel chase in...
  colorChase(strip.Color(127,127,127), 20); // white
  colorChase(strip.Color(127,0,0), 20);     // red
  colorChase(strip.Color(127,127,0), 20);   // yellow
  colorChase(strip.Color(0,127,0), 20);     // green
  colorChase(strip.Color(0,127,127), 20);   // cyan
  colorChase(strip.Color(0,0,127), 20);     // blue
  colorChase(strip.Color(127,0,127), 20);   // magenta
  
  // Blink board LED 5 time when we either get a signal or the button
  // is pressed.
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_0, true);
    delay(50);
    digitalWrite(LED_0, false);
    delay(50);
  }
  
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


// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  }

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c); // set one pixel
      strip.show();              // refresh strip display
      delay(wait);               // hold image for a moment
      strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
  }
  strip.show(); // for last erased pixel
}

// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
void dither(uint32_t c, uint8_t wait) {

  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for(int bit=1; bit < 0x8000; bit <<= 1) {
    if(n & bit) hiBit = bit;
  }

  int bit, reverse;
  for(int i=0; i<(hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for(bit=1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if(i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, c);
    strip.show();
    delay(wait);
  }
  delay(250); // Hold image for 1/4 sec
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 8); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    delay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}

// Sine wave effect
#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    for(int i=0; i<strip.numPixels(); i++) {
      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();
    delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}