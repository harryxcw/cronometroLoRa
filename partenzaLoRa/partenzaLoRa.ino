//    Partenza Lora, uso un TTGO LoRa32-OLED V1
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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//ENRICO
#include <string.h>
#define BOTTONE 13

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;
String lastPacket; //Enrico

//ENRICO
typedef struct atleta {
  String nomeA;
  unsigned long tempo[50] = {0};
  int giroN = 0;
  unsigned long millisStart;
  unsigned long millisFinish;
  bool partito = false;
  bool esiste = false;
  unsigned long sommaTempi = 0;
  int giriConclusi = 0;
} t_atleta;

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



int sincronia = 0;
String stringMillisArrivo = "0"; //quella di inizio sincronizzazione
String stringMillisPassaggio = "0"; //quella che manda l'arrivo al passaggio
unsigned long millisArrivo = 0;
unsigned long millisQuandoArriva = 0;
unsigned long millisQuandoParte = 0;
bool lastBottone = 0; //all'inizio li avevo messi 1
bool bottone = 0;
int atletaSelezionato = 0;
int numeroAtleti = 11; //!!!PROVVISORIO numero totale di iscritti adesso per prova sta a 2 che li ho inseriti manualmente, poi lo tolgo torna a 0
unsigned long cronoProvvisorio = 0;
int atletaInPista = 0;
int volte = 0;
int codaAtleti[5] = {0}; //vettore che descrive quali atleti sono in pista e in che ordine
int numCodaAtleti = 0; //lunghezza di codaAtleti (riempita)
const unsigned long autoDNF = 90000; //valore per DNF automatico (1 min e mezzo)
float scartoDNF = 1.3;//ovvero 30% in più della media tempi

volatile unsigned long int_millis_now = 0;
volatile unsigned long int_millis_last = 0;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int i = 0;
int j = 0;
int k = 0;


t_atleta registroAtleti[100];

void IRAM_ATTR ISR() {
  int_millis_now = millis();
  if (int_millis_now - int_millis_last > 200) {
    if (registroAtleti[atletaSelezionato].partito == false) {

      registroAtleti[atletaSelezionato].millisStart = int_millis_now - millisQuandoArriva;//millis quando arriva non varia, è quello di riceviMillis()
      registroAtleti[atletaSelezionato].partito = true;
      registroAtleti[atletaSelezionato].giroN++;
      //atletaInPista = atletaSelezionato; //questo serve per lasciare memoria di quello che è partito

      codaAtleti[numCodaAtleti] = atletaSelezionato; //creo un vettore di tipo last in last out? sicuro che non è FILO?
      numCodaAtleti++; //l'atleta che dovrebbe arrivare è sempre quello in prima posizione nel vettore
    }
    int_millis_last = int_millis_now;
  }

}

void setup() {
  //ENRICO
  //pin del pulsantino di input
  pinMode(BOTTONE, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  attachInterrupt(25, ISR, RISING);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1, 2);
  display.setCursor(0, 0);
  display.print("BUONGIORNO! ");
  display.display();

  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Crono Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 20);
  display.println("LoRa Initializing OK!");
  display.display();
  delay(2000);

  //!!!PROVVISORIO per la creazione poi tolgo

  registroAtleti[0].nomeA = "Pinco Pallino Joe";
  registroAtleti[0].esiste = true;
  registroAtleti[1].nomeA = "Gian Cane";
  registroAtleti[1].esiste = true;
  registroAtleti[2].nomeA = "Carmela";
  registroAtleti[2].esiste = true;
  registroAtleti[3].nomeA = "Capitano";
  registroAtleti[3].esiste = true;
  registroAtleti[4].nomeA = "Eddy O\'cane";
  registroAtleti[4].esiste = true;
  registroAtleti[5].nomeA = "Mr. White";
  registroAtleti[5].esiste = true;
  registroAtleti[6].nomeA = "Mr. Orange";
  registroAtleti[6].esiste = true;
  registroAtleti[7].nomeA = "Mr. Blonde";
  registroAtleti[7].esiste = true;
  registroAtleti[8].nomeA = "Mr. Pink";
  registroAtleti[8].esiste = true;
  registroAtleti[9].nomeA = "Mr. Brown";
  registroAtleti[9].esiste = true;
  registroAtleti[10].nomeA = "Bill";
  registroAtleti[10].esiste = true;

  //fine poi tolgo
  inputString.reserve(200);

}

void loop() {
  //chiedo sincronia all'arrivo
  if (!sincronia) {
    chiediMillis();
  }
  while (!sincronia) {
    riceviMillis();
    Serial.println("Attesa millis arrivo...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("attesa sincronia..");
    display.display();
    k++;
    if (k > 500) {
      chiediMillis();
      k = 0;
    }
  }


  //serialEvent
  leggiInput();

  //gestione display
  bottone = digitalRead(BOTTONE);
  if (bottone < lastBottone) { //ovvero se è stato premuto (bottone va a 0)
    atletaSelezionato++;
    while (!registroAtleti[atletaSelezionato].esiste) { //ovvero vai avanti fino al successivo che esiste
      atletaSelezionato++;
    }
    lastBottone = bottone;
    if (atletaSelezionato >= numeroAtleti) {
      atletaSelezionato = 0; //cioè quando arriva alla fine della lista ricomincia
    }
  }
  if (bottone > lastBottone) {
    lastBottone = bottone;
  }
  //a questo punto avro nel display selezionato il nome di chi deve
  //partire rappresentato da atletaSelezionato
  if (registroAtleti[codaAtleti[0]].partito == true) { //questo viene attivato dall interrupt, ho sostituito atletaInPista con codaAtleti[0]
    cronoProvvisorio = millis() - registroAtleti[codaAtleti[0]].millisStart - millisQuandoArriva; //togli anche millis quando parte?, la parentesi aggiunta il 5 gen per togliere sfaso
  }

  //inizio ricezione pacchetto
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.println(LoRaData);
    }

    if (lastPacket == LoRaData) { //per evitare che la ridondanza dell'info mi chiude anche quello dietro in pista
      LoRaData = "";
      return;
    }
    lastPacket = LoRaData;

    if ((LoRaData.startsWith(Matricola)) && (registroAtleti[codaAtleti[0]].partito == true)) {// per evitare errori che si richiudeva al ripassaggio, ho sost atletaInPista con codaAtleti[0]
      stringMillisPassaggio = LoRaData.substring(2);

      if ((cronoProvvisorio < 10000) && (cronoProvvisorio > 400)) {
        goto nonAvreiVoluto; //ovvero se risulta un tempo minore a 20s ignora il passaggio
      }


      registroAtleti[codaAtleti[0]].millisFinish = stringMillisPassaggio.toInt() - millisArrivo;
      registroAtleti[codaAtleti[0]].tempo[registroAtleti[codaAtleti[0]].giroN] = registroAtleti[codaAtleti[0]].millisFinish - registroAtleti[codaAtleti[0]].millisStart; //qui viene registrato il tempo del giro (in millisecondi)

      //qui aggiorno la variabile sommaTempi dell'ultimo atleta e il num di giri conclusi, per poi determinare un tempo medio per atleta per restringere l'intervallo del DNF automatico
      registroAtleti[codaAtleti[0]].sommaTempi += registroAtleti[codaAtleti[0]].tempo[registroAtleti[codaAtleti[0]].giroN];
      registroAtleti[codaAtleti[0]].giriConclusi++;





      //mando atleta e tempo su seriale
      Serial.print(registroAtleti[codaAtleti[0]].nomeA);
      Serial.print(" giro ");
      Serial.print(registroAtleti[codaAtleti[0]].giroN);
      Serial.print(" tempo ");
      Serial.println(registroAtleti[codaAtleti[0]].tempo[registroAtleti[codaAtleti[0]].giroN]);


      registroAtleti[codaAtleti[0]].partito = false; //a questo punto ho il tempo salvato e sa che non c'è nessuno in pista
      //devo scalare di uno il vettore degli atleti in pista (ci voleva un for ma faccio così)
      scalaLaCodaAtleti();


      // DA SCRIVERE!! manda il tempo su telegram
    }
  }

nonAvreiVoluto: //lo so non si usano i goto :*)

  //qui attribuisce il DNF all'atleta che deve arrivare
  if (registroAtleti[codaAtleti[0]].giriConclusi == 0) {
    if ((cronoProvvisorio > autoDNF) && (registroAtleti[codaAtleti[0]].partito == true)) {//per evitare che segnasse dnf da solo a 75s
      registroAtleti[codaAtleti[0]].tempo[registroAtleti[codaAtleti[0]].giroN] = -1; //ovvero dnf
      registroAtleti[codaAtleti[0]].partito = false;
      Serial.print(" numcodaatleti: ");
      Serial.println(numCodaAtleti);
      scalaLaCodaAtleti();
      // DA SCRIVERE!! manda il tempo su telegram ovviamente il dnf
    }
  } else { //qui quando ha fatto almeno un giro, la differenza è che il confronto(il primo del if) lo fa con la media tempi e non il valore di autoDNF
    if ((cronoProvvisorio > (scartoDNF * registroAtleti[codaAtleti[0]].sommaTempi / registroAtleti[codaAtleti[0]].giriConclusi)) && (registroAtleti[codaAtleti[0]].partito == true)) { //per evitare che segnasse dnf da solo a 75s
      registroAtleti[codaAtleti[0]].tempo[registroAtleti[codaAtleti[0]].giroN] = -1; //ovvero dnf
      registroAtleti[codaAtleti[0]].partito = false;
      Serial.print(" numcodaatleti: ");
      Serial.println(numCodaAtleti);
      scalaLaCodaAtleti();
      // DA SCRIVERE!! manda il tempo su telegram ovviamente il dnf
    }
  }

  //la parte che gestisce il display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(registroAtleti[atletaSelezionato].nomeA);
  display.setCursor(0, 20);
  //display.print("  ");
  if (registroAtleti[atletaSelezionato].tempo[registroAtleti[atletaSelezionato].giroN] > 0) {//se ha finito almeno un giro
    display.print("Giro ");
    display.print(registroAtleti[atletaSelezionato].giroN);
    display.print("   ");
    display.print(millisToTime(registroAtleti[atletaSelezionato].tempo[registroAtleti[atletaSelezionato].giroN]));

  } else if (registroAtleti[atletaSelezionato].partito) {//se è in pista ora
    display.print(" Giro ");
    display.print(registroAtleti[atletaSelezionato].giroN);
    display.print("   ");
    display.print(millisToTime(millis() - registroAtleti[atletaSelezionato].millisStart - millisQuandoArriva));
  } else if (registroAtleti[atletaSelezionato].giroN > 0) {//se è partito almeno una volta
    display.print("Giro ");
    display.print(registroAtleti[atletaSelezionato].giroN);
    display.print(" DNF");
  }

  //queste sono i 2 giri vecchi, ci saranno val random all'inizio
  if (registroAtleti[atletaSelezionato].giroN > 1) {//era maggiore di 0
    display.setCursor(0, 40);
    display.print("Giro ");
    display.print(registroAtleti[atletaSelezionato].giroN - 1);
    display.print("  ");
    if ((registroAtleti[atletaSelezionato].tempo[registroAtleti[atletaSelezionato].giroN - 1]) != -1) {
      display.print(millisToTime(registroAtleti[atletaSelezionato].tempo[registroAtleti[atletaSelezionato].giroN - 1]));
    } else {
      display.print("DNF");
    }
  }
  //  if (registroAtleti[atletaSelezionato].giroN > 1) { //l'ho commentato tanto non ci entra nel display
  //    display.setCursor(0, 60);
  //    display.print("Giro ");
  //    display.print(registroAtleti[atletaSelezionato].giroN - 2);
  //    display.print("  ");
  //    display.print(millisToTime(registroAtleti[atletaSelezionato].tempo[registroAtleti[atletaSelezionato].giroN - 2]));
  //  }
  //  display.setCursor(20, 40);
  //  display.print("Live: ");
  //  display.print(cronoProvvisorio);
  if (digitalRead(25)) {
    display.setCursor(110, 0);
    display.print("$");//che è il cod ascii per il rettangolo pieno
  }
  display.display();



  delay(10);



}

void scalaLaCodaAtleti() {
  codaAtleti[0] = codaAtleti[1];
  codaAtleti[1] = codaAtleti[2];
  codaAtleti[2] = codaAtleti[3];
  codaAtleti[3] = codaAtleti[4];
  codaAtleti[4] = 0;

  numCodaAtleti--;
}
