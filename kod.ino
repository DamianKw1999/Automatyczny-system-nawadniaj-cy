#include <EEPROM.h>                           // biblioteka używana do zapisywania danych w pamięci arduino
#include <DFRobot_DHT11.h>                    // biblioteka używana do obsługi czujnika temperatury i wilgotności powietrza DHT11
#include <Wire.h>                             // podstawowa biblioteka arduino używana do komunikacji I2C/TWI                  
#include <Adafruit_SH1106.h>                  // biblioteka używana do obsługi wyświetlacza OLED

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);
DFRobot_DHT11 DHT;

unsigned long timer = 0;
const unsigned long interwal = 1000;
int address = 25;                              // deklaracja zmiennych do obsługi wyświetlania wykresów z danych pomiarowych

void setup() {

  EEPROM.begin();                          

  pinMode(PD2, INPUT);
  pinMode(PD3, OUTPUT);
  pinMode(PD4, INPUT);
  pinMode(PD5, OUTPUT);
  pinMode(PD6, OUTPUT);
  pinMode(PD7, OUTPUT);
  pinMode(9, OUTPUT);                       
  Serial.begin(9600);                          // deklaracja używanych pinów i rozpoczęcie transmisji szeregowej

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);    // inicjalizacja ekranu OLED, zdefiniowanie jego adresu 
  delay(2000);
  display.clearDisplay();
}


void loop() {

  int stanprzycisku = digitalRead(PD2);
  if (stanprzycisku == HIGH & digitalRead(9) == LOW) {
    digitalWrite(9, HIGH);
    delay(500);
  } else if (stanprzycisku == HIGH & digitalRead(9) == HIGH)  
  {
    digitalWrite(9, LOW); 
  }                                             // funkcja sprawdzająca stan przycisku w celu przełączenia trybu pracy

START:

  display.clearDisplay();
  DHT.read(PD4);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Temp. otoczenia:");
  display.print(19);
  display.setCursor(0, 10);
  display.print("Wilg. powietrza:");
  display.print(23);
  display.print("%");
  display.setCursor(0, 20);
  display.print("Wilg. gleby:");
  int wilgotnosc = analogRead(A0);
  int wil = map(wilgotnosc, 182, 455, 100, 0);
  display.setTextSize(1);
  display.print(wil);
  display.print("%");
  display.setCursor(0, 30);                     // odczytywanie danych z wejść cyfrowych i analogowych z czujników, przetwarzanie oraz wyświetlanie ich na ekranie

  if (wil < 15) {
    digitalWrite(PD7, LOW);
    display.setCursor(0, 40);
    display.setTextSize(1);
    display.println("Za niska wilgotnosc gleby!!");
    digitalWrite(PD3, HIGH);
    digitalWrite(PD6, HIGH);
    delay(100);
    digitalWrite(PD3, LOW);
    digitalWrite(PD6, LOW);
    digitalWrite(PD5, HIGH);
  }                                              // funkcja odpoiwadająca za działanie urządzenia podczas zbyt niskiego odczytu z czujnika wilgotnosci gleby

  if (wil > 15) {
    digitalWrite(PD6, LOW);
    display.setCursor(0, 40);
    display.setTextSize(1);
    display.print("Wilgotnosc gleby w normie :)");
    digitalWrite(PD7, HIGH);
    digitalWrite(PD5, LOW);
  }                                              // funkcja odpoiwadająca za działanie urządzenia podczas prawidłowego odczytu z czujnika wilgotnosci gleby

  if (millis() - timer > interwal) {             //zapisywanie danych do pamięci EEPROM arduino

    EEPROM.write(address, wil);
    Serial.println(EEPROM.read(address));
    Serial.println(timer);
    Serial.println(address);
    Serial.println(wil);
    address++;
 
    if (address > 124) {                         // resetowanie wartości adresu po zapełnieniu ekranu
      address = 25;
    }
    timer = millis();
  }

  while (digitalRead(9) == HIGH) {               // wyświetlanie wykresu z danych zapisanych w pamięcu arduino
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Wykres wilgotnosci gleby:");
    display.setCursor(0, 54);
    display.print("0%");
    display.setCursor(0, 36);
    display.print("50%");
    display.setCursor(0, 20);
    display.print("100%");
    for (int i = 25; i < 124; i++) {
      int wil1 = EEPROM.read(i);
      int wil2 = map(wil1, 0, 100, 64, 20);
      if (wil2 < 20) {
        wil2 = 64;
      }
      display.setCursor(i, 64);
      display.drawLine(i, 64, i, wil2, 1);
      display.display();
      if (digitalRead(2) == HIGH) {
        digitalWrite(9, LOW);
        display.clearDisplay();
        break;
        goto START;
      }
      delay(50);
    }
  }

  display.display();
  delay(500);
}
