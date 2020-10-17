/*
  Электронный предсказатель погоды по изменению давления
  Измеряет давление каждые 10 минут, находит разницу с давлением час назад
  При расчёте давления использовано среднее арифметическое из 10 измерений
  для уменьшения шумности сигнала с датчика
  AlexGyver 2017
*/


//-----------------------НАСТРОЙКИ---------------------
#define servo_invert 1       // если серва крутится не в ту сторону, изменить значение (1 на 0, 0 на 1)
#define battery_min 3000     // минимальный уровень заряда батареи для отображения
#define battery_max 4200     // максимальный уровень заряда батареи для отображения
// диапазон для 3 пальчиковых/мизинчиковых батареек: 3000 - 4700
// диапазон для одной банки литиевого аккумулятора: 3000 - 4200
//-----------------------НАСТРОЙКИ---------------------

#define servo_Vcc 12           // пин питания, куда подключен мосфет

//------ИНВЕРСИЯ------
#if servo_invert == 1
#define servo_180 0
#define servo_0 180
#else
#define servo_0 0
#define servo_180 180
#endif
//------ИНВЕРСИЯ------

//------БИБЛИОТЕКИ------

//------ИНВЕРСИЯ------

boolean wake_flag, move_arrow;
int sleep_count, angle, delta, last_angle = 90;
float k = 0.8;
float my_vcc_const = 1.067;    // константа вольтметра
unsigned long pressure, aver_pressure, pressure_array[6], time_array[6];
unsigned long sumX, sumY, sumX2, sumXY;
float a, b;




void setup() {
  Serial.begin(9600);
  Serial.println(readVcc());
}
void loop() {
 Serial.println(readVcc());
 delay(500);
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
