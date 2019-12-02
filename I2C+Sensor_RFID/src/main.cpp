#include <Arduino.h>
#include <Wire.h>                   // incluir a biblioteca para comunicação I2C
#include <MFRC522.h>

#define SLAVE1 0x01
// PIN Numbers : RESET + SDAs
#define RST_PIN         9
#define SS_1_PIN        A3

// Number of sensors
#define NR_OF_READERS   1

byte ssPins[] = {SS_1_PIN};

// Create an MFRC522 instance :
MFRC522 mfrc522[NR_OF_READERS];

void request_event();
void receiveEvent(int howMany);
void dump_byte_array(byte * buffer, byte bufferSize);

void setup(){

    Serial.begin(9600);           // Initialize serial communications with the PC
    Wire.begin(SLAVE1);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(request_event);

    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {

    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);

    Serial.print(F("Position "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();

    delay(100);

  }

}

void loop(){

    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {

          // Looking for new cards

          if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
            Serial.print(F("M1"));
            Serial.print(F("P"));
            Serial.println(reader);

            // Show some details of the PICC (that is: the tag/card)

            Serial.print(F("UID: "));
            dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
            Serial.println();



            /*Serial.print(F("PICC type: "));
              MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
              Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/

            // Halt PICC

            mfrc522[reader].PICC_HaltA();

            // Stop encryption on PCD

            mfrc522[reader].PCD_StopCrypto1();

          }

  }

}

void request_event(){
    Wire.write("Dados");    // ----- Ainda tem que mandar os dados certo do sensor -----
}

void receiveEvent(int howMany){                 // Função que é executada automaticamente assim que o slave

                                             // receber dados do master
    String inString = "";              // limpar inString para impedir concatenar dados atuais com anteriores
    while(Wire.available() >= 1){    // lê até o final da string enviada pelo primeiro Wire.write() do master
        char inChar = (char)Wire.read(); // Wire.read() recebe cada byte da string de bytes
        inString += inChar;             // armazanar, concatenando, cada byte recebido do master

                                                // colocando-os numa variável String
    }
}

void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}