#include <UTFT.h>
#include <MFRC522.h>
#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11
#define sensorControlPin 7
#define sensorDataPin A1
#define button1Pin 2
#define button2Pin 3
#define button3Pin 4
#define greutateGhiozdan 400

struct {
  char nume[2][20];
  char numeOra[20];
  int rfid[2][4];
  int kilo[2];
  bool pus[2];
} orar[5] = {
  {
    {"caiet romana", "manual romana"},
    "romana",
    {{0x1C, 0x60, 0x6A, 0xB3}, {0x7C, 0xEB, 0x71, 0xB3}},
    {100, 250},
    {false, false}
  },
  {
    {"caiet matematica", "manual matematica"},
    "matematica",
    {{0xBA, 0x35, 0x08, 0x4C}, {0x2C, 0x9C, 0x7F, 0xB3}},
    {89, 167},
    {false, false}
  },
  {
    {"caiet fizica", "manual fizica"},
    "fizica",
    {{0x00, 0x01, 0x02, 0x03}, {0x11, 0x12, 0x13, 0x14}},
    {59, 177},
    {false, false}
  },
  {
    {"caiet chimie", "manual chimie"},
    "chimie",
    {{0x22, 0x23, 0x24, 0x25}, {0x33, 0x34, 0x35, 0x36}},
    {145, 251},
    {false, false}
  },
  {
    {"caiet germana", "manual germana"},
    "germana",
    {{0x44, 0x45, 0x46, 0x47}, {0x55, 0x56, 0x57, 0x58}},
    {133, 197},
    {false, false}
  },
};

MFRC522 rfid(10, 9);
MFRC522::MIFARE_Key key;
UTFT myGLCD(CTE32HR, 38, 39, 40, 41);
DHT dht(DHTPIN, DHTTYPE);
extern unsigned short backpack[];
extern uint8_t BigFont[];
unsigned long curTime, prevTime, powerCycle;
boolean powerState;
int currMenu = 0, oldHumi = -1, oldTemp = -1, oldGas = -10;
int Tkilo = greutateGhiozdan, oldReady, readySchool = -1, oldTkilo = -1;

void printLCD(int colorRed, int colorGreen, int colorBlue, char* message, int posX, int posY);
void printLCD(int colorRed, int colorGreen, int colorBlue, int message, int posX, int posY);

void setup() {
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0 , 0, 0);
  myGLCD.setBackColor(0, 0, 0);
  printLCD(255, 0, 0, "Salut!", 100, 100);
  printLCD(200, 200, 0, "Eu sunt TechBag,", 130, 120);
  printLCD(255, 255, 0, "Companionul tau !", 130, 140);
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  dht.begin();
  pinMode(sensorControlPin, OUTPUT);
  pinMode(sensorDataPin, INPUT);
  curTime = 0;
  prevTime = 1;
  powerCycle = 0;
  powerState = LOW;
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  delay(2000);
  myGLCD.clrScr();
}

void loop() {
  oldTkilo = Tkilo;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    if (rfid.uid.uidByte[0] != 0) {
      actualizare();
      for (int i = 0; i < rfid.uid.size; i++)
        rfid.uid.uidByte[i] = 0;
    }
  }
  if (digitalRead(2) == HIGH) {
    if (currMenu != 0) {
      myGLCD.clrScr();
    }
    currMenu = 0;
  }
  else if (digitalRead(3) == HIGH && currMenu == 0) {
    myGLCD.clrScr();
    currMenu = 1;
  }
  else if (digitalRead(4) == HIGH && currMenu == 0) {
    myGLCD.clrScr();
    currMenu = 2;
  }
  if (currMenu == 0) {
    printLCD(255, 180, 0, "Main Menu", 150, 0);
    printLCD(114, 203, 23, "1) Orar", 0, 20);
    printLCD(114, 203, 23, "Aici afli materiile tale", 20, 40);
    printLCD(114, 203, 23, "din fiecare zi", 20, 60);
    printLCD(254, 68, 201, "2) Mediul tau inconjurator", 0, 180);
    printLCD(254, 68, 201, "Aici iti ofer informatii", 20, 200);
    printLCD(254, 68, 201, "referitoare la temperatura,", 20, 220);
    printLCD(254, 68, 201, "umiditatea si calitatea", 20, 240);
    printLCD(254, 68, 201, "aerului din jurul tau", 20, 260);
  }
  else if (currMenu == 1) {
    printLCD(255, 255, 255, "Orarul tau", 144, 10);
    printLCD(255, 190, 100, "08:00-09:00 ", 0, 50);
    printLCD(255, 255, 255, orar[0].numeOra, 200, 50);
    if (orar[0].pus[0]&orar[0].pus[1] == true) {
      printLCD(0, 255, 0, "OK", 390, 50);
    }
    else {
      printLCD(255, 0, 0, " X", 390, 50);
    }
    printLCD(255, 190, 100, "09:00-10:00 ", 0, 90);
    printLCD(255, 255, 255, orar[1].numeOra, 200, 90);
    if (orar[1].pus[0]&orar[1].pus[1] == true) {
      printLCD(0, 255, 0, "OK", 390, 90);
    }
    else {
      printLCD(255, 0, 0, " X", 390, 90);
    }
    printLCD(255, 190, 100, "10:00-11:00 ", 0, 130);
    printLCD(255, 255, 255, orar[2].numeOra, 200, 130);
    if (orar[2].pus[0]&orar[2].pus[1] == true) {
      printLCD(0, 255, 0, "OK", 390, 130);
    }
    else {
      printLCD(255, 0, 0, " X", 390, 130);
    }
    printLCD(255, 190, 100, "11:00-12:00 ", 0, 170);
    printLCD(255, 255, 255, orar[3].numeOra, 200, 170);
    if (orar[3].pus[0]&orar[3].pus[1] == true) {
      printLCD(0, 255, 0, "OK", 390, 170);
    }
    else {
      printLCD(255, 0, 0, " X", 390, 170);
    }
    printLCD(255, 190, 100, "12:00-13:00 ", 0, 210);
    printLCD(255, 255, 255, orar[4].numeOra, 200, 210);
    if (orar[4].pus[0]&orar[4].pus[1] == true) {
      printLCD(0, 255, 0, "OK", 390, 210);
    }
    else {
      printLCD(255, 0, 0, " X", 390, 210);
    }
    oldReady = readySchool;
    readySchool = 1;
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 2; j++) {
        if (orar[i].pus[j] == false) {
          readySchool = 0;
          break;
        }
      }
    }
    if (readySchool != oldReady) {
      myGLCD.setColor(0, 0, 0);
      myGLCD.print("                            ", 0, 250);
      myGLCD.setColor(255, 255, 255);
      if (readySchool == 0) {
        myGLCD.print("Mai ai de pus carti!", 0, 250);
      }
      else {
        myGLCD.print("Hai la scoala!", 0, 250);
      }
    }
    printLCD(255, 255, 255, "Greutate: ", 0, 270);
    if (oldTkilo != Tkilo) {
      printLCD(0, 0, 0, "         ", 200, 270);
    }
    printLCD(255, 255, 255, Tkilo, 200, 270);
  }
  else if (currMenu == 2) {
    printLCD(58, 219, 195, "Mediul tau inconjurator", 55, 0);
    int humi = dht.readHumidity();
    if (oldHumi != humi) {
      oldHumi = humi;
      printLCD(44, 150, 220, "Umiditate=", 0, 20);
      printLCD(0, 0, 0, "     ", 170, 20);
    }
    else {
      printLCD(44, 150, 220, "Umiditate=", 0, 20);
    }
    printLCD(44, 150, 220, humi, 170, 20);
    int temp = dht.readTemperature();
    if (oldTemp != temp) {
      oldTemp = temp;
      printLCD(193, 208, 10, "Temperatura=", 0, 40);
      printLCD(0, 0, 0, "     ", 204, 40);
    }
    else {
      printLCD(193, 208, 10, "Temperatura=", 0, 40);
    }
    printLCD(193, 208, 10, temp, 204, 40);
    int gas = gasRead();
    if (oldGas != gas) {
      oldGas = gas;
      printLCD(56, 179, 139, "Calitatea aerului=", 0, 60);
      printLCD(0, 0, 0, "        ", 326, 60);
    }
    else {
      printLCD(56, 179, 139, "Calitatea aerului=", 0, 60);
    }
    if (gas == -1) {
      printLCD(255, 0, 0, "n/a", 326, 60);
    }
    else {
      printLCD(56, 179, 139, gas, 326, 60);
    }
  }
}
int gasRead() {
  curTime = millis();
  if (curTime - prevTime > powerCycle) {
    prevTime = curTime;
    if (powerState == LOW) {
      powerState = HIGH;
      powerCycle = 60000;
    }
    else {
      powerState = LOW;
      powerCycle = 90000;
    }
    digitalWrite(sensorControlPin, powerState);
  }
  if (powerState == LOW) {
    int coValue = analogRead(sensorDataPin);
    return coValue;
  }
  else {
    return -1;
  }
}
void printLCD(int colorRed, int colorGreen, int colorBlue, char* message, int posX, int posY)
{
  myGLCD.setColor(colorRed, colorGreen, colorBlue);
  String thisString = String(message);
  char charBuf[50];
  thisString.toCharArray(charBuf, 50);
  myGLCD.print(charBuf, posX, posY);
}
void printLCD(int colorRed, int colorGreen, int colorBlue, int message, int posX, int posY)
{
  myGLCD.setColor(colorRed, colorGreen, colorBlue);
  String thisString = String(message);
  char charBuf[50];
  thisString.toCharArray(charBuf, 50);
  myGLCD.print(charBuf, posX, posY);
}
void actualizare() {
  for (int i = 0; i < 5; i++)
  {
    int ok;
    int j = -1;
    ok = 1;
    for (int k = 0; k < rfid.uid.size; k++) {
      if (orar[i].rfid[0][k] != rfid.uid.uidByte[k]) {
        ok = 0;
        break;
      }
    }
    if (ok == 1) {
      j = 0;
    }
    ok = 1;
    for (int k = 0; k < rfid.uid.size; k++) {
      if (orar[i].rfid[1][k] != rfid.uid.uidByte[k]) {
        ok = 0;
        break;
      }
    }
    if (ok == 1) {
      j = 1;
    }


    if (j != -1)
    {
      myGLCD.setColor(255, 255, 255);
      if (orar[i].pus[j] == false)
      {
        orar[i].pus[j] = true;
        myGLCD.print("                                                      ", 0, 230);
        myGLCD.print("Introdus ", 0, 230);
        myGLCD.print(orar[i].nume[j], 160, 230);
        Tkilo += orar[i].kilo[j];
      }
      else
      {
        orar[i].pus[j] = false;
        myGLCD.print("                                                      ", 0, 230);
        myGLCD.print("Extras ", 0, 230);
        myGLCD.print(orar[i].nume[j], 130, 230);
        Tkilo -= orar[i].kilo[j];
      }
    }
  }

}
