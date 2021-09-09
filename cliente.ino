#include <SPI.h>
#include <MFRC522.h>

#define TIMEOUT_INTRA_MANO 2000
unsigned long lastReadingTime = 0;
String lastReading = "";

#define numberOfCardsIds 66
#define RST_PIN  5    //Pin  9 para el reset del RC522
#define SS_PIN  4   //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

byte buzzPin = D0; //Esto equivale al pin D0 del NodeMCU

String ArrayOfCards[] = {"7C678DC0", "B71C813F", "2E80307A", "3ED8A37B", "5EF66CAF", "2EAF9BAF", "5E85C8AF", "7E98C5AF", "3E78EF7A", "0E92DB7B", "AE049BAF", "1E75C8AF", "8C1885C0", "7CC487C0", "1CEE8CC0", "FCDF8DC0", "7CE287C0", "CC3195C0", "2E83757A", "5C2E8CC0", "BC7884C0", "9C6C8CC0", "5EA5CA5B", "DE6DA65B", "EEAF997B", "CE8F827A", "601E9B16", "70547416", "40AD3216", "10B79816", "B0579416", "E0737716", "20757116", "00657116", "B0007116", "307F9916", "D0301616", "40877416", "80AC8D16", "B02D6616", "40653616", "60D39816", "408D6616", "20B59816", "10402F16", "F0051516", "307F7416", "A0B87216", "97E0F5F2", "30C2932C", "40A76816", "E0FE7716", "80994016", "50B09816", "709D9516", "60357116", "00BC9816", "D0755732", "801C7016", "10082416", "003E6916", "D0B11716", "20909816", "10A06F16", "A0D73916", "40023516"};

void setup() {
  Serial.begin(9600); //Iniciamos la comunicación  serial
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos  el MFRC522
  pinMode (buzzPin, OUTPUT); // Configuramos pin de salida para el buzzer
  Serial.println("Lectura del UID");
  pinMode (D3, OUTPUT);
  pinMode (D8, OUTPUT);

  // ESP.wdtDisable();
}

void loop() {
  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent())
  {
    //Seleccionamos una tarjeta
    if ( mfrc522.PICC_ReadCardSerial())
    {
      // Enviamos serialemente su UID
      Serial.print("Card UID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      char str[32] = "";
      bytes_to_string(mfrc522.uid.uidByte, 4, str);
      // Terminamos la lectura de la tarjeta  actual
      mfrc522.PICC_HaltA();
      if (lastReading != str || millis() - lastReadingTime > TIMEOUT_INTRA_MANO) {
        buzzbeep();  // Hacer sonido de recibido
        bool esColor = compareCards(str);
        reportarLectura(esColor);
      }

      lastReading = str;
      lastReadingTime = millis();
    }
  }
}

//Función para comparar dos tarjetas
boolean compareCards(String strTemp)
{
  for (int i = 0; i < numberOfCardsIds; i++) {
    if (strTemp == ArrayOfCards[i])
      return (true);
  }
  return (false);
}

void reportarLectura (bool correcta) {
  digitalWrite(D8, correcta);
  digitalWrite(D3, HIGH);
  delay(5);
  digitalWrite(D3, LOW);
}


//Función para convertir bytes to string
void bytes_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

  //Sonido del beep

  void buzzbeep() {
  tone(buzzPin, 932.33, 80);
  }
