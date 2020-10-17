/*
   Скетч для калибровки точного вольтметра и его использование
   КАЛИБРОВКА:
   0) Ставим vol_calibration 1, прошиваем
   1) Запускаем, открываем монитор. Будет выведено реальное значение Vсс в милливольтах
   из расчёта по стандартной константе 1.1
   2) Измеряем вольтметром напряжение на пинах 5V и GND, полученное значение отправляем в порт
   В МИЛЛИВОЛЬТАХ (то есть если у нас 4.54В то отправляем 4540). Новая константа будет рассчитана
   автоматически и запишется во внутреннюю память
   3) Ставим vol_calibration 0, прошиваем
   4) Наслаждайтесь точными измерениями!
   ИСПОЛЬЗОВАНИЕ:
   0) Функция readVCC возвращает ТОЧНОЕ опорное напряжение В МИЛЛИВОЛЬТАХ. В расчётах используем
   не 5 Вольт, а readVсс!
   1) При использовании analogRead() для перевода в вольты пишем:
   float voltage = analogRead(pin) * (readVсс() / 1023.0); - это точный вольтаж В МИЛЛИВОЛЬТАХ
*/
#define vol_calibration 1    // калибровка вольтметра (если работа от АКБ) 1 - включить, 0 - выключить
float my_vcc_const = 1.1;    // начальное значение константы должно быть 1.1
#include "EEPROMex.h"        // библиотека для работы со внутренней памятью ардуино
void setup() {
  Serial.begin(9600);
  if (vol_calibration) calibration();     // калибровка, если разрешена
  my_vcc_const = EEPROM.readFloat(1000);  // считать константу из памяти
}
void loop() {
  /*
    // отображение заряда в процентах по ёмкости! Интерполировано
 вручную по графику разряда ЛИТИЕВОГО аккумулятора
    int volts = readVcc();
    int capacity;
    if (volts > 3870)
    capacity = map(volts, 4200, 3870, 100, 77);
    else if ((volts <= 3870) && (volts > 3750) )
    capacity = map(volts, 3870, 3750, 77, 54);
    else if ((volts <= 3750) && (volts > 3680) )
    capacity = map(volts, 3750, 3680, 54, 31);
    else if ((volts <= 3680) && (volts > 3400) )
    capacity = map(volts, 3680, 3400, 31, 8);
    else if (volts <= 3400)
    capacity = map(volts, 3400, 2600, 8, 0);
    Serial.println(capacity);
  */
}
void calibration() {
  my_vcc_const = 1.1;                                           // начальаня константа калибровки
  Serial.print("Real VCC is: "); Serial.println(readVcc());     // общаемся с пользователем
  Serial.println("Write your VCC (in millivolts)");
  while (Serial.available() == 0); int Vcc = Serial.parseInt(); // напряжение от пользователя
  float real_const = (float)1.1 * Vcc / readVcc();              // расчёт константы
  Serial.print("New voltage constant: "); Serial.println(real_const, 3);
  Serial.println("Set vol_calibration 0, flash and enjoy!");
  EEPROM.writeFloat(1000, real_const);                          // запись в EEPROM
  while (1);                                                    // уйти в бесконечный цикл
}
// функция чтения внутреннего опорного напряжения, универсальная (для всех ардуин)
long readVcc() {
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
