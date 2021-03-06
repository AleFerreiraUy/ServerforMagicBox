#include <SPI.h>
#include <MFRC522.h>
#define CANT_CARTAS 2
#define numberOfCardsIds 74

#define TIMEOUT_INTRA_MANO 2000
#define TIMEOUT_INTERRUPTION 200
unsigned long lastReadingTime = 0;
String lastReading = "";

unsigned long lastInterruptionTime = 0;
#define RST_PIN  5    //Pin  9 para el reset del RC522
#define SS_PIN  4   //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

byte buzzPin = D0; //Equivale al D4 de los pines del NodeMCU

String ArrayOfCards[] = {"4EE58E7B", "2C428CC0", "DC078DC0", "BC848BC0", "9E8B6CAF", "AE2570AF", "D74B4E40", "9B2C2C21", "271E6540", "4C4C87C0", "FC8592C0", "4CD58CC0", "1E3B0B7B", "DEC3A77B", "AE59DC7A", "9EA7A77B", "0C6534FC", "5EF0087B", "CE130B7B", "6EE0627B", "9CE4C2C1", "AC29C3C1", "1E8E0496", "0E520A96", "AE8E477B", "CCC3DFFB", "7E624B7B", "4C4D86C0", "AEA5937B", "8ED3A37B", "2E0B997B", "5E440A2D", "7EB3797A", "8EC03C7B", "004E7116", "709C9B16", "E0BD7C2C", "F0C59816", "703A4016", "10B61416", "70773916", "00757416", "D04A9B16", "C0632516", "80DD6E16", "80937716", "50F00B16", "60937316", "A0148E16", "20B30C16", "903D9916", "70497116", "D00B7016", "00826916", "707A0C16", "C0252416", "70827316", "40712F16", "80E19416", "50812316", "90077116", "C09C9016", "50253C16", "D0E01516", "906B9916", "A0673016", "B0231516", "10E06E16", "102E0F16", "F0603016", "504D7416", "100F8C16", "00DB9916", "40579416"};


int lecturas = 0;
bool correcto = true;

void ICACHE_RAM_ATTR test() {
  if (millis() - lastInterruptionTime > TIMEOUT_INTERRUPTION) {
    if (digitalRead (D8) ) {
      Serial.println("correcto");
      correcto = correcto && true;
    } else {
      Serial.println("erroneo");
      correcto = false;
    }
    lecturas ++;
    lastInterruptionTime = millis();
  }
}
void setup() {
  //noInterrupts();
  // put your setup code here, to run once:
  Serial.begin (9600);
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos  el MFRC522
  pinMode(buzzPin, OUTPUT); // Seteo del pin del buzzer como salida
  pinMode (D8, INPUT);
  pinMode (D3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D3), test, RISING);
  delay (10000);
  lecturas = 0;
  Serial.println ("Initialized...");
  tone(buzzPin, 932 , 40);
 // interrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent())
  { Serial.println("que ande");
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
      Serial.println(str);
      if (lastReading != str || millis() - lastReadingTime > TIMEOUT_INTRA_MANO) {
        tone(buzzPin, 932 , 80);    // Hacer sonido de recibido
        lecturas ++;
        Serial.println(lecturas);
        bool esColor = compareCards(str);
        correcto = correcto && esColor;
      }

      lastReading = str;
      lastReadingTime = millis();
    }
  }

  if (lecturas >= CANT_CARTAS) {
    if (correcto) {
      Serial.println("Ambas correctas");
      buzzHappy();
    } else {
      Serial.println("Son erroneas");
      buzzSad();
      buzzSad();
      buzzSad();

    }
    correcto = true;
    lecturas = 0;
  }
}

//Funci??n para convertir bytes to string
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

//Funci??n para comparar dos tarjetas
boolean compareCards(String strTemp)
{
  for (int i = 0; i < numberOfCardsIds; i++) {
    if (strTemp == ArrayOfCards[i])
      return (true);
  }
  return (false);
}


//Funci??n para sonidos

//Sonido feliz, triste y beep

void buzzbeep() {
  tone(buzzPin, 932.33, 80);
}

void buzzHappy() {
  delay(500);
  tone(buzzPin, 1567, 121); delay(43);
  tone(buzzPin, 1567, 121); delay(43);
  tone(buzzPin, 1864.5, 212); delay(107);
  tone(buzzPin, 1864.5, 121); delay(42);
  tone(buzzPin, 1864.5, 121); delay(42);
  tone(buzzPin, 2093, 121); delay(41);
  tone(buzzPin, 2093, 121); delay(41);
  tone(buzzPin, 2093, 121); delay(41);
  tone(buzzPin, 2093, 121); delay(41);
  tone(buzzPin, 2635, 212); delay(104);
  tone(buzzPin, 2093, 212); delay(104);
  tone(buzzPin, 2349, 543);
}


void buzzSad() {
  delay(500);
  float c = 0;
  float maxValueOfc = 2.0;

  while (c < maxValueOfc) {



    c = c + (0.001); //es un line que deber??a ir de 0 a 4.3
    float a = -20 * c; //hace que la frecuencia vaya de 543hz a 456hz
    float x = c * 6.283; //6.283 = 2pi
    float b = (cos(x) * 100); //oscilaci??n modulante
    tone(buzzPin, ((543 + a) + b), 500);
  }
}
