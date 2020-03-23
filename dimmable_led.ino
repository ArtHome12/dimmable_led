/* ===============================================================================
Управление диммируемым освещением.
1 February 2020.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2020 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include <Bounce2.h>
#include <EEPROM.h>
#include <HTU21D.h>

// Для отладки - если определено, в Serial выводятся сообщения.
#define DEBUG


// Адреса пинов
const int buttonPin = 2;  // номер входа, подключенный к кнопке
const int ledPin =  13;   // номер выхода светодиода
const int driverPin = 3;  // номер выхода на драйвер (транзистор).    
 
// Поддерживаемые события
enum Events {
  eClick,                 // отжатие после щелчка
  eLongPress,             // долгое нажатие
  eLongPressFree,         // отпуск после долгого нажатия
  eDoubleClick            // отжатие после двойного щелчка
};

unsigned long previousMillis = 0;               // Момент последнего обновления
const unsigned long maxClickDelay = 500;        // Максимальное время для короткого щелчка, мс.
const unsigned long longPressFirstDelay = 1000; // Время до первого события bLongPress, мс.
const unsigned long longPressRepeatDelay = 10;  // Время между повторными событиями bLongPress, мс.
unsigned long previousClickMillis = 0;          // Время последнего короткого щелчка (для генерации двойного), мс.
const unsigned long temperatureControlDelay = 12000;  // Время между измерениями температуры, мс.
unsigned long temperatureControlMillis = 0;           // Время последнего измерения температуры, мс.

bool isBPressed = false;                        // Истина, когда кнопка нажата.  

const int eepromAddrPWM = 0;        // Адрес для хранения в EEPROM признака завершения работы.
byte PWMLevel = 0;                  // Текущий уровень на ШИМ
const byte PWMLevelMinBright = 210; // Минимальная яркость (с запасом, светится до 223)
const byte PWMLevelHalfBright = 75;// Половинная яркость (с запасом, светится до 223)
bool increaseUp = true;             // Направление увеличения яркости при повторении LongPress.
bool powerOn = false;               // Включен или нет светодиод.

const float cWarningTemp = 50.0;    // Температура, при которой надо убавить яркость наполовину.
const float cCriticalTemp = 60.0;   // Температура, при которой надо убавить яркость на минимум.

// Для подавления дребезга
Bounce debouncer = Bounce();

HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);        // Интерфейс к датчику температуры и влажности.


void setup() {
  // Разгон частоты ШИМ. Пины D3 и D11 - 62.5 кГц https://alexgyver.ru/lessons/pwm-overclock/
#ifndef __INTELLISENSE__    // Обходим глюк интеллисенса, не понимающего include внутри ifdef.
  TCCR2B = 0b00000001; // x1
  TCCR2A = 0b00000011; // fast pwm
#endif

#if defined(DEBUG)
  // initialize serial communication:
  Serial.begin(115200);
  Serial.println("Start...");
#endif

  // Прочитаем прежнее значение яркости из EEPROM
  PWMLevel = EEPROM.read(eepromAddrPWM);

  // Пин на драйвер, сразу гасим лампу.
  pinMode(driverPin, OUTPUT);
  analogWrite(driverPin, 255);

  // инициализируем пин, подключенный к светодиоду, как выход
  pinMode(ledPin, OUTPUT);     
  // инициализируем пин, подключенный к кнопке с защитой от дребезга.
  debouncer.attach(buttonPin, INPUT_PULLUP);

  // Датчик температуры.
  myHTU21D.begin();

  // Пропускаем возможный дребезг из-за включения.
  delay(600);
  debouncer.update();
}
 
void loop() {

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
    // Сохраняем новое состояние.
    isBPressed = bCurState;

    // Если кнопка была отжата.
    if (!isBPressed) {
      // Если кнопка была отжата быстро, значит произошёл щелчок.
      if (deltaTime < maxClickDelay) {
        // Отправляем событие либо щелчок либо двойной щелчок.
        event(currentMillis - previousClickMillis < longPressFirstDelay ? eDoubleClick : eClick);

        // Сохраняем время щелчка для выявления двойного щелчка.
        previousClickMillis = currentMillis;
      } else {
        // Если кнопка была отжата после долговременного нажатия.
        event(eLongPressFree);
      } 
    } 
  } else {
    // Если состояние не менялось.

    // Если кнопка в нажатом состоянии и прошло достаточно времени, надо посылать долгое нажатие.
    if (isBPressed) { 
      if (deltaTime > longPressFirstDelay) {
        event(eLongPress);

        // Откорректируем время так, чтобы повторные длинные нажатия происходили быстро.
        previousMillis = currentMillis - longPressFirstDelay + longPressRepeatDelay;
      }
    } else
      // Если кнопка в отжатом состоянии, сбрасываем время.
      previousMillis = currentMillis;
  }

  // Контроль температуры
  if (currentMillis - temperatureControlMillis > temperatureControlDelay) {
    
    // Сохраним время текущего контроля температуры.
    temperatureControlMillis = currentMillis;

    // Прочитаем температуру с датчика (+-0.3C).
    float temp = myHTU21D.readTemperature();

    // Определим максимально допустимую яркость.
    byte maxAllowedBright = temp > cCriticalTemp ? PWMLevelMinBright : (temp > cWarningTemp ? PWMLevelHalfBright : 0);
    
    // Ограничим максимальную яркость.
    if (PWMLevel < maxAllowedBright)
      analogWrite(driverPin, maxAllowedBright);
      
#if defined(DEBUG)
      Serial.print("Temperature ");   Serial.print(temp); 
      Serial.print("C, maxAllowedBright ");   Serial.print(maxAllowedBright);
      Serial.print(" PWMLevel ");   Serial.println(PWMLevel);
#endif
  }


  // Индикация на светодиоде.
  digitalWrite(ledPin, isBPressed ? HIGH : LOW);
}

void event(Events bCommand) {
  switch (bCommand)
  {
    case eClick:
#if defined(DEBUG)
      Serial.println("Click");
#endif
      // Переключим освещение.
      powerOn = !powerOn;
      if (powerOn) {
        // Для гарантированного включения должна быть яркость не ниже минимальной.
        PWMLevel = PWMLevel > PWMLevelMinBright ? PWMLevelMinBright : PWMLevel;

        // После любого переключения при удержании кнопки яркость сначала увеличивается.
        increaseUp = false;

        // Управляем драйвером.
        analogWrite(driverPin, PWMLevel);
        EEPROM.write(eepromAddrPWM, PWMLevel);
    } else
        // Выключение.
        analogWrite(driverPin, 255);
        EEPROM.write(eepromAddrPWM, 255);
    break;
  
  case eLongPress:
    // Если лимпа выключена, надо начать с минимальной яркости.
    if (!powerOn) {
      PWMLevel =  PWMLevelMinBright;
      increaseUp = false;
      powerOn = true;
      analogWrite(driverPin, PWMLevel);

      // Задержка, чтобы человек успел отреагировать на включение и убрать палец.
      delay(longPressFirstDelay);
      break;
    }

    // Изменяем значение уровня.
    if (increaseUp) {
      if (PWMLevel == 255) {
        PWMLevel = 254;
        increaseUp = false;
      } else {
          PWMLevel++;
      }
    } else {
      if (PWMLevel == 0) {
        PWMLevel = 1;
        increaseUp = true;
      } else {
        PWMLevel--;
      }
    }

    // Выдаём управляющий сигнал - за счёт перебора значений ниже минимальной яркости будет удобная пауза.
#if defined(DEBUG)
    Serial.print("PWM Level ");    Serial.println(int(PWMLevel));
#endif
    analogWrite(driverPin, PWMLevel > PWMLevelMinBright ? PWMLevelMinBright : PWMLevel);

    break;
  
  case eLongPressFree:
#if defined(DEBUG)
    Serial.println("Store PWM");
#endif
    EEPROM.write(eepromAddrPWM, PWMLevel);
    break;

  default: // eDoubleClick
#if defined(DEBUG)
    Serial.println("Double click");
#endif
    // Если лампа включена к повторному щелчку, т.е. была выключена при первом щелчке, включим её с максимальной яркостью
    if (powerOn) {
      PWMLevel = 0;
      analogWrite(driverPin, PWMLevel);
    } else {
      // Если лампа выключена, приглушим её до минимальной яркости
      PWMLevel =  PWMLevelMinBright;
      increaseUp = false;
      powerOn = true;
      analogWrite(driverPin, PWMLevel);
    }
    EEPROM.write(eepromAddrPWM, PWMLevel);
    break;
  }
}
