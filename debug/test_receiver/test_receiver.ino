// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12


#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

// RH_ASK driver;
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2
RH_ASK driver(1200 , 11, 12, 0); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85),
// RH_ASK driver(2000, PD14, PD13, 0); STM32F4 Discovery: see tx and rx on Orange and Red LEDS



void setup()
{

  Serial.begin(1200);   // Debugging only


  if (!driver.init())
    Serial.println("init failed");
  else
    Serial.println("init ok");


}

void loop()
{

  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) // Non-blocking
  {
    int i;

    // Message with a good checksum received, dump it.
    driver.printBuffer("Got:", buf, buflen);
    Serial.print(buflen);
    Serial.println(String((char*)buf));
    Serial.println(buf[buflen-1]);
    if (buf[buflen-1] == '1') {
      digitalWrite(LED_BUILTIN, HIGH);
    }// turn the LED on (HIGH is the voltage level)
    else {
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    }
  }
}
