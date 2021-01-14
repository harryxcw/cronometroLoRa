//    Arrivo Lora, uso un TTGO promini_LORA_V02 (20190506)
//    Copyright (C) 2020  Enrico Percivalli
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//    This is free software, and you are welcome to redistribute it
//    under certain conditions;

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

//ENRICO
#include <string.h>
#define ASSEX A3
#define ASSEY A6
#define ASSEZ A7

//define the pins used by the LoRa transceiver module
#define SCK 13 //era 5
#define MISO 12 //era 19
#define MOSI 11 //era 27
#define SS 10 //era 18
#define RST 9 //era 14
#define DIO0 2 //era 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
//#define OLED_SDA 4
//#define OLED_SCL 15
//#define OLED_RST 16
//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels


//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

//ENRICO
//##############################################################
//##############################################################
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// CAMBIALA!!!! MUST CHANGE THIS!!!!

String Matricola = "AS";


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//##############################################################
//##############################################################

unsigned long millisRicevuto = 0;

int acceX = 0;//sono le analogread delle accellerazioni
int acceY = 0;
int acceZ = 0;
int oldAcceX = 0;
int oldAcceY = 0;
int oldAcceZ = 0;
unsigned long oldMicros = 0; //poi faccio il modulo di tutto e derivata
unsigned long deltaAcce = 0; //e vedo se ha preso la botta
unsigned long goodMillis = 0;
int velocita = 2000; //era 5 ma ho modificato il deltaAcce con i quardrati


void setup() {

  //reset OLED display via software
  //  pinMode(OLED_RST, OUTPUT);
  //  digitalWrite(OLED_RST, LOW);
  //  delay(20);
  //  digitalWrite(OLED_RST, HIGH);
  //
  //initialize OLED
  //  Wire.begin(OLED_SDA, OLED_SCL);
  //  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
  //    Serial.println(F("SSD1306 allocation failed"));
  //    for(;;); // Don't proceed, loop forever
  //  }

  //  display.clearDisplay();
  //  display.setTextColor(WHITE);
  //  display.setTextSize(1);
  //  display.setCursor(0,0);
  //  display.print("BUONGIORNO! ");
  //  display.display();

  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Crono Test");

  //SPI LoRa pins
  //SPI.begin(SCK, MISO, MOSI, SS); //ENRICO non era commentato, misa giusto così che nell'altra andavano cambiati qua no
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1); //per debug va commentata
  }
  Serial.println("LoRa Initializing OK!");
  //  display.setCursor(0,10);
  //  display.println("LoRa Initializing OK!");
  //  display.display();
  delay(2000);
}

void loop() {

  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");
    Serial.println(rssi);

    // se e quando arriva un pacchetto contenente mmm
    //significa che lo start ha richiesto il millis
    //che gli mando
    if (LoRaData.startsWith(Matricola)) {
      //invio attuale millis
      delay(1000);
      Serial.println("invio millis..");
      LoRa.beginPacket();
      LoRa.print(Matricola);
      LoRa.print(millis());
      LoRa.endPacket();
      delay(500);
      LoRa.beginPacket();
      LoRa.print(Matricola);
      LoRa.print(millis());
      LoRa.endPacket();
      //loop di prova
      //      display.clearDisplay();
      //      display.setCursor(0,0);
      //      display.print("millis mandato!");
      //      display.display();
      millisRicevuto = millis();
    }


  }
  //  if (millisRicevuto>0){
  //    display.clearDisplay();
  //    display.setCursor(0,0);
  //    display.print((millis()-millisRicevuto)/1000);
  //    display.print(" 8==D");
  //    display.display();
  //  }

  //inizio accelerometro
  acceX = analogRead(ASSEX);
  acceY = analogRead(ASSEY);
  acceZ = analogRead(ASSEZ);
  if ((micros() - oldMicros) > 0) {
    goodMillis = millis();
    deltaAcce = (sq(acceX - oldAcceX) + sq(acceY - oldAcceY) + sq(acceZ - oldAcceZ)) / (micros() - oldMicros); // in origine avevo messo abs invece che sq
  }
  //Serial.println(deltaAcce);

  oldAcceX = acceX;
  oldAcceY = acceY;
  oldAcceZ = acceZ;
  oldMicros = micros();
  delay(20);
  //  display.clearDisplay();
  //  display.setCursor(0,0);
  //  display.print("X ");
  //  display.print(acceX);
  //  display.setCursor(20,20);
  //  display.print("Y ");
  //  display.print(acceY);
  //  display.setCursor(40,40);
  //  display.print("Z ");
  //  display.print(acceZ);
  //  display.setCursor(50,0);
  //  display.print("DELTA ");
  //  display.print(deltaAcce);
  //  display.display();
  //fine accelerometro

  if (deltaAcce > velocita) { // manda MMmillis quando delta acce supera un valore soglia
  LoRa.beginPacket();
    LoRa.print(Matricola);
    LoRa.print(goodMillis);//oldMillis è il tempo mills quando ha rilevato il deltaAcce
    LoRa.endPacket();
    delay(500);
    LoRa.beginPacket();
    LoRa.print(Matricola);
    LoRa.print(goodMillis);
    LoRa.endPacket();
    Serial.print("passaggio rilevato!! con deltaAcce= ");
    Serial.println(deltaAcce);

    Serial.print("X= ");
    Serial.println(acceX);
    Serial.print("Y= ");
    Serial.println(acceY);
    Serial.print("Z= ");
    Serial.println(acceZ);

    delay(6000);//per evitare richiusura per tipo rimbalzo palo
  }


}
