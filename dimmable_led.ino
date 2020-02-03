/* ===============================================================================
Управление диммируемым освещением.
1 February 2020.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2020 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include <Bounce2.h>;

// задаем константы
const int buttonPin = 2;     // номер входа, подключенный к кнопке
const int ledPin =  13;      // номер выхода светодиода
 
// Команды
const int bClick = 1;         // отжатие после щелчка
const int bLongPress = 2;     // долгое нажатие
const int bDobleClick = 3;    // двойной щелчок

unsigned long previousMillis = 0;       // Момент последнего обновления
const long maxClickDelay = 1000;         // Минимально необходимый интервал для работы внутри loop(), мс.

bool isBPressed = false;                // Истина, когда кнопка нажата.  

// Для подавления дребезга
Bounce debouncer = Bounce();

void setup() {
  // инициализируем пин, подключенный к светодиоду, как выход
  pinMode(ledPin, OUTPUT);     
  // инициализируем пин, подключенный к кнопке, как вход
  pinMode(buttonPin, INPUT);  

  debouncer.attach(buttonPin); 

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  //previousMillis = millis();
}
 
void loop(){

  // Пропускаем дребезг.
  debouncer.update();

	// Текущее время.
	unsigned long currentMillis = millis();

  // считываем значения с входа кнопки
  bool bCurState = debouncer.read() == HIGH;
 
  // Если произошло изменение состояния кнопки
  if (bCurState != isBPressed) {
    isBPressed = bCurState;

    // Если кнопка была отжата ранее наступления длинного интервала, произошёл щелчок.
    if (!isBPressed) {

      // Условие вычисляем отдельно, для защиты от перехода через 0.
      unsigned long condition = currentMillis - previousMillis;

      if (condition < maxClickDelay)
        event(bClick);

      previousMillis = currentMillis;
    }

  }
  
  // если нажата, то buttonState будет HIGH:
  if (isBPressed) {   
    // включаем светодиод   
     digitalWrite(ledPin, HIGH);
  }
  else {
    // выключаем светодиод
    digitalWrite(ledPin, LOW);
  }
}

void event(int bCommand) {
  Serial.println(bCommand);
}