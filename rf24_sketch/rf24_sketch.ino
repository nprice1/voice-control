
/*

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 /*
  Hack.lenotta.com
  Modified code of Getting Started RF24 Library
  It will switch a relay on if receive a message with text 1, 
  turn it off otherwise.
  Edo
 */

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            8

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//
// Hardware conf
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
//ce and csn
RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


char * convertNumberIntoArray(unsigned short number, unsigned short length) {
    
    char * arr = (char *) malloc(length * sizeof(char)), * curr = arr;
    do {
        *curr++ = number % 10;
      number /= 10;
    } while (number != 0);
    return arr;
}

unsigned short getId(char * rawMessage, unsigned short length){
    unsigned short i = 0;
    unsigned short id = 0;
    for( i=1; i< length; i++){
        id += rawMessage[i]*pow( 10, i-1 );
    }
    return id;
}

unsigned short getMessage( char * rawMessage){
    unsigned short message = rawMessage[0];
    return (unsigned short)message;
}
unsigned short getLength( unsigned int rudeMessage){
    unsigned short length = (unsigned short)(log10((float)rudeMessage)) + 1;
    return length;
}

void setColor(uint16_t red, uint16_t green, uint16_t blue) {
  for(byte i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(red,  green,   blue));
    pixels.show();
  }
}

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  pinMode(PIN, OUTPUT);
  printf_begin();
  printf("\nRemote Switch Arduino\n\r");
  
  // setup led matrix
  pixels.begin();
  pixels.setBrightness(255);  // range: 0 ~ 255 
  setColor(0,0,0);

  //
  // Setup and configure rf radio
  //

  radio.begin();
//  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(15,15);

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
  radio.printDetails();
}

int getState(unsigned short pin){
  boolean state = digitalRead(pin);
  return state == true ? 0 : 1;
}

void doAction(unsigned short id, unsigned short action){
    if ( action == 1 ){
      // red
      setColor(255, 0, 0);
    } else if (action == 2) {
      // green
      setColor(0, 255, 0);
    } else if (action == 3) {
      // blue
      setColor(0, 0, 255); 
    } else if (action == 4) {
      // yellow
      setColor(255, 255, 0); 
    } else if (action == 5) {
      // teal
      setColor(0, 255, 255); 
    } else if (action == 6) {
      // purple
      setColor(255, 0, 255); 
    } else if (action == 7) {
      // white
      setColor(255, 255, 255); 
    } else if (action == 8) {
      // off
      setColor(0, 0, 0); 
    }
}

void sendCallback(unsigned short callback){
   // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &callback, sizeof(unsigned short) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
}

void performAction(unsigned short rawMessage){
  unsigned short action, id, length, callback;
  char * castedMessage;
  
  length = getLength(rawMessage);
  castedMessage = convertNumberIntoArray(rawMessage, length);
  action = getMessage(castedMessage);
  id = getId(castedMessage, length);

  doAction(id, action);
  sendCallback(callback);
}
void loop(void)
{
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned short message;
      bool done;
//      char * new;
      unsigned short rawMessage; 
      done = false;
      while ( radio.available() )
      {
        // Fetch the payload, and see if this was the last one.
        radio.read( &rawMessage, sizeof(unsigned long) );

        // Spew it
        printf("Got message %d...",rawMessage); 

        performAction(rawMessage);

        delay(10);
      }

     
    }
}
