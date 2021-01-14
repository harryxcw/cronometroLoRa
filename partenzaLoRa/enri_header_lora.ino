//questa funzione serve a richiede indietro all'arrivo il suo millis

void chiediMillis () {
  Serial.println("chiedo millis arrivo...");
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(Matricola);
  LoRa.print("mmm");
  LoRa.endPacket();
  millisQuandoParte = millis();
  delay(500);
  LoRa.beginPacket();
  LoRa.print(Matricola);
  LoRa.print("mmm");
  LoRa.endPacket();
}

void riceviMillis() {
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  int stringLegth = -1;
  if (packetSize) {
    //received a packet
    Serial.print("(riceviMillis)Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      stringLegth = LoRaData.length();
      if (LoRaData.startsWith(Matricola)) {
        stringMillisArrivo = LoRaData.substring(2);
        millisArrivo = stringMillisArrivo.toInt();
        millisQuandoArriva = millis();
        //millisQuandoArriva = millisQuandoArriva - ((millisQuandoArriva - millisQuandoParte) / 2); // qui va messo in sync cronoProvvisorio
        sincronia = 1;
        delay(1000);

      }

    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");
    Serial.print(rssi);
    Serial.print(" at ");
    Serial.print(millis());
    Serial.print(" and division ");
    Serial.println((millis() - millisQuandoArriva) / 1000);

  }
}

String Spaces(int numOfSpaces) {
  String mieString;
  int k;
  for (k = 0; k < numOfSpaces; k++) {
    mieString += " ";
  }
  return mieString;
}

String millisToTime(int inputMillis) {
  String mioTempo;
  mioTempo += (int)inputMillis / 60000;
  mioTempo += "\' ";
  mioTempo += (int)(inputMillis % 60000) / 1000;
  mioTempo += "\" ";
  mioTempo += ((int)(inputMillis % 60000) % 1000) / 10;
  return mioTempo;
}
