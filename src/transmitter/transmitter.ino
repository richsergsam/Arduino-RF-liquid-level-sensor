//----------------------БИБЛИОТЕКИ---------------------
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

#include <RH_ASK.h>


//-----------------------НАСТРОЙКИ---------------------
#define battery_min 3000     // минимальный уровень заряда батареи для отображения
#define battery_max 4200     // максимальный уровень заряда батареи для отображения
float my_vcc_const = 1.067;    // константа вольтметра
byte sensor_pin = 7;  // Подтяжка через резистор 10КОм к VCC для режима INPUT
byte prescaler = PRESCALER_4;
byte clock_multiplier = pow(2, prescaler);
unsigned int serial_and_rf_clock = 1200;

void setup() {
  
  power.setSystemPrescaler(prescaler);
  Serial.begin(serial_and_rf_clock * clock_multiplier);   // Debugging only
  Serial.println("Current internal clock: " + String(F_CPU / clock_multiplier / 1000000) + " MGhz;");
  power.setSleepMode(POWERDOWN_SLEEP);
  

}

void loop() {
  // Send sensors values
  checkSensorAndSendData();
  // Wait 10 sec
  power.sleepDelay(5*1000);
}


void checkSensorAndSendData(){
  long vcc = readVcc();
  pinMode(sensor_pin, INPUT_PULLUP);
  byte sensor_status = digitalRead(sensor_pin);
  pinMode(sensor_pin, INPUT);
  Serial.println(sendVccAndSensor(vcc, sensor_status));
}


String sendVccAndSensor(long vcc, int sensor_status) {
  String msg = String(vcc) + "; " + String(sensor_status);
  sendData(msg);
  return msg;
}

void sendData(String message) {
  RH_ASK driver(serial_and_rf_clock * clock_multiplier, 11, 12, 0);
  if (!driver.init())
    Serial.println("RF driver init failed");
  else
    Serial.println("RF driver init ok");
  int l = message.length() + 1;
  char msg_buffer[l];
  message.toCharArray(msg_buffer, l);
  driver.send((uint8_t *)msg_buffer, strlen(msg_buffer));
  driver.waitPacketSent();
}

long readVcc() { //функция чтения внутреннего опорного напряжения, универсальная (для всех ардуин)
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
  long result = (high << 8) | low;

  result = my_vcc_const * 1023 * 1000 / result; // расчёт реального VCC
  return result; // возвращает VCC
}
