void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
}
void loop() {
  // выведет 0, если кнопка нажата
  // и 1, если нет
  Serial.println(digitalRead(3));
  delay(10);
}
