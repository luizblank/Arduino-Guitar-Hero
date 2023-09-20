#include "FirebaseESP32.h"
#include "WiFi.h"
#include <LiquidCrystal.h>
#include <string>

#define WIFI_NOME "luiz"
#define WIFI_SENHA "s3nh4asa"

#define FB_HOST "https://led-guitar-hero-default-rtdb.firebaseio.com/"
#define FB_AUTH "yO6dWAoONNa4YCZjKH56AQHFiMeWEAigWZS381ah"

FirebaseJson json;
FirebaseData firebaseData;

#define LEDPOS1 0
#define LEDPOS2 0
#define LEDPOS3 0
#define LEDPOS4 0
#define LEDPOS5 0

#define LEDNEG1 0
#define LEDNEG2 0
#define LEDNEG3 0
#define LEDNEG4 0
#define LEDNEG5 0
#define LEDNEG6 0
#define LEDNEG7 0

#define BUTTON1 0
#define BUTTON2 0
#define BUTTON3 0
#define BUTTON4 0
#define BUTTON5 0

const uint8_t LEDSPOS[] = {LEDPOS1, LEDPOS2, LEDPOS3, LEDPOS4, LEDPOS5};
const uint8_t LEDSNEG[] = {LEDNEG1, LEDNEG2, LEDNEG3, LEDNEG4, LEDNEG5, LEDNEG6, LEDNEG7};
const uint8_t BUTTONS[] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5};

int linhas = 6;
int colunas = 5;

int count = 0;
int points = 0;
int fails = 0;

LiquidCrystal lcd(0, 0, 0, 0, 0, 0);

void printLCD(char* linha1, char* linha2, bool x1 = true, bool x2 = true){
  int xcentral1 = 0;
  int xcentral2 = 0;

  if(x1)
    xcentral1 = centralIndex(linha1);
  if(x2)
    xcentral2 = centralIndex(linha2);

  lcd.clear();
  for(int i = 0; i < 16; i++)
  {
    lcd.setCursor(i + xcentral1, 0);
    lcd.print(linha1[i]);
  }
  
  for(int i = 0; i < 16; i++)
  {
    lcd.setCursor(i + xcentral2, 1);
    lcd.print(linha2[i]);
  }
}

void printDefault(){
  printLCD("GUITAR HERO", "Pontuacao:", true, false);
  lcd.setCursor(11, 1);
  lcd.print(points); 
}

void printRestart(){
  printLCD("Aperte um botao", "para reiniciar");
}

void resetAndPrintLoad(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Carregando...");
  count = 0;
  points = 0;
  fails = 0;
  delay(1000);
  
  printDefault();
}

int centralIndex(char* linha){ 
  int x = ((16 - sizeof(linha)) / 2) - 1;
  return x;
}

bool verifyCorrect(int8_t notas){
  for(int i = 0; i < colunas; i++)
  {
    int8_t op = 1 << i;
    bool logica = notas && op > 0;
    if(digitalRead(LEDSPOS[i]) != logica)
      return false;
  }

  return true;
}

bool TurnOnLed(int8_t notas, int time){
  for(int i = 0; i < linhas; i++)
  {
    // Controla qual entrada (colunas) vai passar energia positiva constante
    for(int j = 0; j < colunas; j++)
    {
      int8_t op = 1 << i;
      bool logica = notas && op > 0;
      digitalWrite(LEDSPOS[j], (LEDSPOS[j] == logica)); 
    }

    // Controla qual linha que vai ficar acesa e quais vão ficar apagadas
    for(int k = 0; k < linhas; k++)
      digitalWrite(LEDSNEG[k], (LEDSNEG[k] != LEDSNEG[i]));
    
    delay(time);
    
    int realindex = linhas - 1;
    bool verificacao = verifyCorrect(notas);

    if(i == realindex && verificacao == true)
    {
      points += 100;
      
      printLCD("DEUS DO RITMO!", "Pontuacao: +100", true, false);
      delay(500);
      printDefault();
      
      break;
    }
    
    else if(i != realindex && verificacao == false)
    {
      if((points -= 50) <= 0)
      	points = 0;
      else
      	points -= 50;

      fails += 1;
      
      printLCD("Ta adiantado ai?", "Pontuacao: -50", true, false);
      delay(500);
      printDefault();
      
      break;
    }
    
    else if(fails == 5)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GAME OVER!");
        delay(1000);
        printRestart();

        while(1)
        {
          for(int i = 0; i < colunas; i++)
            if(digitalRead(BUTTONS[i]) == HIGH)
            {
              resetAndPrintLoad();
              return 0;
            }
        }
    }
  }

  return 1;
}

void setup(){
  // Iniciando LCD
  lcd.begin(16, 2);

  // Iniciando Serial
  Serial.begin(115200);

  // pinMode Negativos
  for(int i = 0; i < linhas; i++){
    pinMode(LEDSNEG[i], OUTPUT);
  }

  // pinMode Positivos e Botões
  for(int i = 0; i < colunas; i++){
    pinMode(LEDSPOS[i], OUTPUT);
    pinMode(BUTTONS[i], INPUT);
  }

  // Conectando o WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.begin(WIFI_NOME, WIFI_SENHA);
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  // Conectando Banco de Dados
  Firebase.begin(FB_HOST, FB_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "large");
}

void loop(){
  while(count == 0)
  {
    printLCD("/ GUITAR \\", "\\  HERO  /");
    delay(1000);
    printLCD("Pressione para", "jogar...");
    delay(1000);

    for(int i = 0; i < colunas; i++)
      if(digitalRead(BUTTONS[i]) == HIGH)
      {
        printLCD("Carregando...", "");
        delay(1000);
        printLCD("3", "");
        delay(200);
        printLCD("2", "");
        delay(200);
        printLCD("1", "");
        delay(200);
        count = 1;
      }
  }

  while(count == 1)
  {
    if(TurnOnLed(0b01010, 500) == 0){ 
      return;
    }
    if(TurnOnLed(0b10101, 300) == 0){ 
      return;
    }
    if(TurnOnLed(0b10000, 500) == 0){ 
      return;
    }
    if(TurnOnLed(0b00101, 300) == 0){ 
      return;
    }
    
    count = 2;
    break;
  }

  if(count == 2){
    printLCD("WHAT A SHOW!", "Pontuacao:", true, false);
    lcd.setCursor(11, 1);
    lcd.print(points); 
    delay(1000);
    printRestart();
    
    while(1){
      for(int i = 0; i < colunas; i++){
        if(digitalRead(BUTTONS[i]) == HIGH){
          resetAndPrintLoad();
          return;
        }
      }
    }
  }
}