
//String inputString = "";         // a String to hold incoming data
//bool stringComplete = false;  // whether the string is complete
//int i = 0;
//int j = 0;

void leggiInput() {
  serialEvent();
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString.startsWith("\\")) {//"\" è il carattere di controllo del bot
      inputString = inputString.substring(1);
      inputString.trim();//serve a togliere il \n che ti mette il serial.read

      if (inputString.startsWith("add")) { //aggiungi un atleta
        inputString = inputString.substring(4);
        registroAtleti[numeroAtleti].nomeA = inputString;
        registroAtleti[numeroAtleti].esiste = true;
        numeroAtleti++;
        Serial.print(inputString);
        Serial.println(" aggiunto!");
      }//end aggiungi atleta

      if (inputString.startsWith("remove")) { //rimuovi un atleta
        inputString = inputString.substring(7); //a questo punto inputString è il nome di quello da togliere

        for (i = -1; i <= numeroAtleti; ++i) { //che stile il ++i ;)
          if (inputString.equals(registroAtleti[i].nomeA)) {
            registroAtleti[i].esiste = false;
            Serial.print(inputString);
            Serial.println(" rimosso!");
            break;
          }
          if (i = numeroAtleti) {
            Serial.print(inputString);
            Serial.println(" non trovato!");
          }
        }
      }//end rimuovi atleta

      if (inputString.startsWith("lista")) {//start lista
        for (i = 0; i < numeroAtleti; i++) {
          Serial.println(registroAtleti[i].nomeA);
          for (j = 1; j <= registroAtleti[i].giroN; j++) {
            Serial.print(Spaces(registroAtleti[i].nomeA.length())); //serialSpaces l'ho scritta io in header
            Serial.print(" giro: ");
            Serial.print(j);
            Serial.print(" ");
            if (registroAtleti[i].tempo[j] == -1) {
              Serial.print("DNF");
            } else {
              Serial.print(registroAtleti[i].tempo[j]);//con j++ qua c'era +1
            }
            Serial.println();
          }
        }
      }//end lista


    }



    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}



void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  //Serial.println(inputString);
  //Serial.println(stringComplete);
}
