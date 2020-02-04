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

unsigned long previousMillis = 0;         // Момент последнего обновления
const unsigned long maxClickDelay = 500;  // Максимальное время для короткого щелчка, мс.
const unsigned long longPressFirstDelay = 1000; // Время до первого события bLongPress, мс.
const unsigned long longPressRepeatDelay = 50; // Время между повторными событиями bLongPress, мс.
unsigned long previousClickMillis = 0;    // Время последнего короткого щелчка (для генерации двойного), мс.

bool isBPressed = false;                // Истина, когда кнопка нажата.  

// Для подавления дребезга
Bounce debouncer = Bounce();

void setup() {
  // инициализируем пин, подключенный к светодиоду, как выход
  pinMode(ledPin, OUTPUT);     
  // инициализируем пин, подключенный к кнопке, как вход
  debouncer.attach(buttonPin, INPUT_PULLUP); 

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
  bool bCurState = debouncer.read() == LOW;
 
  // Для удобства.
  unsigned long deltaTime = currentMillis - previousMillis;

  // Если произошло изменение состояния кнопки
  if (bCurState != isBPressed) {
    isBPressed = bCurState;

    // Если кнопка была отжата.
    if (!isBPressed) {
      // Если кнопка была отжата ранее наступления интервала, произошёл щелчок.
      if (deltaTime < maxClickDelay) {
        // Отправляем событие либо щелчок либо двойной щелчок.
        event(currentMillis - previousClickMillis < longPressFirstDelay ? bDobleClick : bClick);

        // Сохраняем время.
        previousClickMillis = currentMillis;
      }
    } 
  } else {
    // Если состояние не менялось.

    // Если кнопка в нажатом состоянии, и прошло достаточно времени, надо посылать долгое нажатие.
    if (isBPressed) { 
      if (deltaTime > longPressFirstDelay) {
        event(bLongPress);

        // Откорректируем время так, чтобы повторные длинные нажатия происходили быстро.
        previousMillis = currentMillis - longPressFirstDelay + longPressRepeatDelay;
      }
    } else
      // Если кнопка в отжатом состоянии, сбрасываем время.
      previousMillis = currentMillis;
  }

  // Индикация на светодиоде.
  digitalWrite(ledPin, isBPressed ? HIGH : LOW);
}

void event(int bCommand) {
  Serial.println(bCommand);
}