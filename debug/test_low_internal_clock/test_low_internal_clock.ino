// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

// пример управления системной частотой
// мы можем только уменьшить (разделить) системную частоту (на платах ардуино 16 МГц)
// Пониженная частота позволяет чуть снизить потребление или питать МК от пониженного напряжения!
#include "GyverPower.h"

// с понижением частоты "уйдут" функции времени
// для их коррекции можно сделать так:
#define millis() (millis() << (CLKPR & 0xF))
#define micros() (micros() << (CLKPR & 0xF))
#define delay(x) delay((x) >> (CLKPR & 0xf))
#define delayMicroseconds(x) delayMicroseconds((x) >> (CLKPR & 0xf))
// данные дефайны нужно прописать ПЕРЕД подключением остальных библиотек.
// Таким образом дефайн сможет "проникнуть" в библиотеку и скорректировать
// работу используемых там функций времени



int prescaler = PRESCALER_4;
int clock_multiplier = pow(2, prescaler);



void setup()
{
  power.setSystemPrescaler(prescaler);
  Serial.begin(300 * clock_multiplier);   // Debugging only
  Serial.println("Current internal clock: " + String(F_CPU / clock_multiplier / 1000000) + " MGhz;");
  
 
}

void loop()
{
  Serial.println("Hello");
  delay(1000);
}
