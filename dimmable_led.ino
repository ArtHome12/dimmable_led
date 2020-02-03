/* ===============================================================================
Управление диммируемым освещением.
1 February 2020.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2020 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

// задаем константы
const int buttonPin = 2;     // номер входа, подключенный к кнопке
const int ledPin =  13;      // номер выхода светодиода
 
// переменные
int buttonState = 0;         // переменная для хранения состояния кнопки
 
// Команды
const int bClick = 1;         // отжатие после щелчка
const int bLongPress = 2;     // долгое нажатие
const int bDobleClick = 3;    // двойной щелчок


void setup() {
  // инициализируем пин, подключенный к светодиоду, как выход
  pinMode(ledPin, OUTPUT);     
  // инициализируем пин, подключенный к кнопке, как вход
  pinMode(buttonPin, INPUT);   

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}
 
void loop(){
  // считываем значения с входа кнопки
  buttonState = digitalRead(buttonPin);
 
  // проверяем нажата ли кнопка
  // если нажата, то buttonState будет HIGH:
  if (buttonState == HIGH) {   
    // включаем светодиод   
    digitalWrite(ledPin, HIGH); 
  }
  else {
    // выключаем светодиод
    digitalWrite(ledPin, LOW);
  }
  // Serial.println(sensorValue);
}
