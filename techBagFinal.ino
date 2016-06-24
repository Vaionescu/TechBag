#include <UTFT.h>
#include <MFRC522.h>
#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11
#define NRMATERII 10
#define NORE 5
#define sensorControlPin 7
#define sensorDataPin A1

struct Orar {
  char nume[20];
  int ptr;
};

MFRC522 rfid(10, 9);
MFRC522::MIFARE_Key key;
extern unsigned short backpack[];
extern uint8_t BigFont[];
UTFT myGLCD(CTE32HR, 38, 39, 40, 41);
DHT dht(DHTPIN, DHTTYPE);
unsigned long curTime, prevTime, powerCycle;
boolean powerState;

byte cartele[NRMATERII][4] = {
  {0, 0, 0, 0},
  {1, 1, 1, 1},
  {0x5B, 0x57, 0x83, 0xF7},
  {0xBA, 0x35, 0x08, 0x4C},
  {0x7C, 0xEB, 0x71, 0xB3},
  {0x1C, 0x60, 0x6A, 0xB3},
  {0x2C, 0x9C, 0x7F, 0xB3},
  {4, 4, 4, 4},
  {2, 2, 2, 2},
  {3, 3, 3, 3}
};

char materii[NRMATERII][20] = {
  {"caiet romana"},
  {"manual romana"},
  {"caiet matematica"},
  {"manual matematica"},
  {"caiet germana"},
  {"manual germana"},
  {"caiet fizica"},
  {"manual fizica"},
  {"caiet chimie"},
  {"manual chimie"}
};

Orar orar[5][5] = {
  {
    {"romana", 0},
    {"romana", 0},
    {"matematica", 1},
    {"fizica", 3},
    {"chimie", 4}
  },
  {
    {"chimie", 4},
    {"germana", 2},
    {"germana", 2},
    {"romana", 0},
    {"romana", 0}
  },
  {
    {"matematica", 1},
    {"matematica", 1},
    {"romana", 0},
    {"fizica", 3},
    {"fizica", 3}
  },
  {
    {"germana", 2},
    {"fizica", 3},
    {"romana", 0},
    {"matematica", 1},
    {"chimie", 4}
  },
  {
    {"germana", 2},
    {"romana", 0},
    {"romana", 0},
    {"matematica", 1},
    {"fizica", 3}
  }
};

bool stare[NRMATERII];

bool puseStare[NRMATERII];

int curr_menu = 2, oldHumi = -1, oldTemp = -1, oldGas = -10;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myGLCD.InitLCD();
  IntroLCD();
  dht.begin();
  gasBegin();
}

void loop() {
  viewMenu(curr_menu);
}

void gasBegin() {
  pinMode(sensorControlPin, OUTPUT);
  pinMode(sensorDataPin, INPUT);
  curTime = 0;
  prevTime = 1;
  powerCycle = 0;
  powerState = LOW;
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
  else{
    return -1;
  }
}

void viewMenu(int noMenu)
{
  if (noMenu == 0) {
    mainMenu();
  }
  else if (noMenu == 1) {
    afisOrar();
  }
  else if (noMenu == 2) {
    afisareUmiTemp();
  }
}

void textOnLCD(int colorRed, int colorGreen, int colorBlue, int message, int posX, int posY)
{
  myGLCD.setColor(colorRed, colorGreen, colorBlue);
  String thisString = String(message);
  char charBuf[50];
  thisString.toCharArray(charBuf, 50);
  myGLCD.print(charBuf, posX, posY);
}
void textOnLCD(int colorRed, int colorGreen, int colorBlue, char* message, int posX, int posY)
{
  myGLCD.setColor(colorRed, colorGreen, colorBlue);
  String thisString = String(message);
  char charBuf[50];
  thisString.toCharArray(charBuf, 50);
  myGLCD.print(charBuf, posX, posY);
}

void IntroLCD()
{
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0 , 0, 0);
  myGLCD.setBackColor(0, 0, 0);
  textOnLCD(255, 0, 0, "Salut!", 100, 100);
  textOnLCD(200, 200, 0, "Eu sunt TechBag,", 130, 120);
  textOnLCD(255, 255, 0, "Companionul tau !", 130, 140);
  delay(2000);
  myGLCD.clrScr();
}

void mainMenu() {
  textOnLCD(255, 180, 0, "Main Menu", 150, 0);
  textOnLCD(114, 203, 23, "1) Orar", 0, 20);
  textOnLCD(114, 203, 23, "Aici afli materiile tale", 20, 40);
  textOnLCD(114, 203, 23, "din fiecare zi", 20, 60);
  textOnLCD(254, 68, 201, "2) Mediul tau inconjurator", 0, 180);
  textOnLCD(254, 68, 201, "Aici iti ofer informatii", 20, 200);
  textOnLCD(254, 68, 201, "referitoare la temperatura,", 20, 220);
  textOnLCD(254, 68, 201, "umiditatea si calitatea", 20, 240);
  textOnLCD(254, 68, 201, "aerului din jurul tau", 20, 260);
}

void afisareUmiTemp()
{
  int humi = dht.readHumidity();
  if (oldHumi != humi) {
    oldHumi = humi;
    textOnLCD(255, 255, 255, "humi=     ", 0, 0);
  }
  else {
    textOnLCD(255, 255, 255, "humi=", 0, 0);
  }
  textOnLCD(255, 255, 255, humi, 80, 0);
  int temp = dht.readTemperature();
  if (oldTemp != temp) {
    oldTemp = temp;
    textOnLCD(255, 255, 255, "temp=     ", 0, 20);
  }
  else {
    textOnLCD(255, 255, 255, "temp=", 0, 20);
  }
  textOnLCD(255, 255, 255, temp, 80, 20);
  int gas = gasRead();
  if (oldGas != gas) {
    oldGas = gas;
    textOnLCD(255, 255, 255, "gas=             ", 0, 40);
  }
  else {
    textOnLCD(255, 255, 255, "gas=", 0, 40);
  }
  if (gas==-1){
    textOnLCD(255, 0, 0, "n/a", 80, 40);
  }
  else{
    textOnLCD(255, 255, 255, gas, 80, 40);
  }
}
bool readySchool = true, oldReady;
void afisOrar() {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Orarul tau", 144, 10);

  myGLCD.setColor(255, 190, 100); myGLCD.print("08:00-09:00 ", 0, 50); myGLCD.setColor(255, 255, 255); myGLCD.print(orar[0][0].nume, 200, 50);
  checkOk(0, 0, 50);
  myGLCD.setColor(255, 0, 0); myGLCD.print("X", 390, 50); myGLCD.setColor(255, 255, 255);

  myGLCD.setColor(255, 190, 100); myGLCD.print("09:00-10:00 ", 0, 90); myGLCD.setColor(255, 255, 255); myGLCD.print(orar[0][1].nume, 200, 90);
  checkOk(0, 1, 90);
  myGLCD.setColor(255, 0, 0); myGLCD.print("X", 390, 90); myGLCD.setColor(255, 255, 255);

  myGLCD.setColor(255, 190, 100); myGLCD.print("10:00-11:00 ", 0, 130); myGLCD.setColor(255, 255, 255); myGLCD.print(orar[0][2].nume, 200, 130);
  checkOk(0, 2, 130);
  myGLCD.setColor(255, 0, 0); myGLCD.print("X", 390, 130); myGLCD.setColor(255, 255, 255);

  myGLCD.setColor(255, 190, 100); myGLCD.print("11:00-12:00 ", 0, 170); myGLCD.setColor(255, 255, 255); myGLCD.print(orar[0][3].nume, 200, 170);
  checkOk(0, 3, 170);
  myGLCD.setColor(255, 0, 0); myGLCD.print("X", 390, 170); myGLCD.setColor(255, 255, 255);

  myGLCD.setColor(255, 190, 100); myGLCD.print("12:00-13:00 ", 0, 210); myGLCD.setColor(255, 255, 255); myGLCD.print(orar[0][4].nume, 200, 210);
  checkOk(0, 4, 210);
  myGLCD.setColor(255, 0, 0); myGLCD.print("X", 390, 210); myGLCD.setColor(255, 255, 255);

  oldReady = readySchool;
  readySchool = true;
  for (int i = 0; i < 5; i++) {
    if (puseStare[i] == false) {
      readySchool = false;
      break;
    }
  }
  if (readySchool != oldReady) {
    myGLCD.setColor(0, 0, 0);
    myGLCD.print("                            ", 0, 290);
    myGLCD.setColor(255, 255, 255);
    if (readySchool == false) {
      myGLCD.print("Mai ai de pus carti!", 0, 290);
    }
    else {
      myGLCD.print("Hai la scoala!", 0, 290);
    }
  }
}
void checkOk(int zi, int ora, int len) {
  if (stare[2 * (orar[zi][ora].ptr + 1) - 1] == true && stare[2 * (orar[zi][ora].ptr + 1) - 2] == true) {
    if (puseStare[ora] == false) {
      myGLCD.setColor(0, 0, 0);
      myGLCD.print("      ", 390, len);
      myGLCD.setColor(0, 255, 0);
      myGLCD.print("OK", 390, len);
      puseStare[ora] = true;
    }
  }
  else {
    if (puseStare[ora] == true) {
      myGLCD.setColor(0, 0, 0);
      myGLCD.print("      ", 390, len);
      myGLCD.setColor(255, 0, 0);
      myGLCD.print("X", 390, len);
      puseStare[ora] = false;
    }
  }
  myGLCD.setColor(255, 255, 255);
}
void citireCartela()
{
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  if (rfid.uid.uidByte[0] != 0) {
    actualizare();
    for (int i = 0; i < rfid.uid.size; i++)
      rfid.uid.uidByte[i] = 0;
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void actualizare() {
  for (int i = 0; i < NRMATERII; i++)
  {
    int ok = 1;
    for (int j = 0; j < rfid.uid.size; j++)
      if (cartele[i][j] != rfid.uid.uidByte[j])
        ok = 0;

    if (ok == 1)
    {
      if (stare[i] == false)
      {
        stare[i] = true;
        myGLCD.print("                                                      ", 0, 250);
        myGLCD.print("Introdus ", 0, 250);
        myGLCD.print(materii[i], 160, 250);
      }
      else
      {
        stare[i] = false;
        myGLCD.print("                                                      ", 0, 250);
        myGLCD.print("Extras ", 0, 250);
        myGLCD.print(materii[i], 130, 250);
      }
    }
  }

}
