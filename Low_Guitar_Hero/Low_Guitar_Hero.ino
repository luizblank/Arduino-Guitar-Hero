#include "FirebaseESP32.h"
#include "WiFi.h"
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include <string>

#define WIFI_NOME "luiz"
#define WIFI_SENHA "s3nh4asa"

#define FB_HOST "https://led-guitar-hero-default-rtdb.firebaseio.com/"
#define FB_AUTH "yO6dWAoONNa4YCZjKH56AQHFiMeWEAigWZS381ah"

FirebaseJson json;
FirebaseData firebaseData;

#define LEDPOS1 15
#define LEDPOS2 2
#define LEDPOS3 4
#define LEDPOS4 16
#define LEDPOS5 17

#define LEDNEG1 34
#define LEDNEG2 35
#define LEDNEG3 32
#define LEDNEG4 33
#define LEDNEG5 25
#define LEDNEG6 26
#define LEDNEG7 27

#define BUTTON1 14
#define BUTTON2 12
#define BUTTON3 13
#define BUTTON4 22
#define BUTTON5 23

#define SIZE_LCD 20

const uint8_t LEDSPOS[] = {LEDPOS1, LEDPOS2, LEDPOS3, LEDPOS4, LEDPOS5};
const uint8_t LEDSNEG[] = {LEDNEG1, LEDNEG2, LEDNEG3, LEDNEG4, LEDNEG5, LEDNEG6, LEDNEG7};
const uint8_t BUTTONS[] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5};

int linhas = 7;
int colunas = 5;


uint8_t count = 0;
int points = 0;
uint8_t fails = 0;

LiquidCrystal_I2C lcd(0x27, 20, 2);

void printDefault()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("GUITAR HERO");

  lcd.setCursor(0, 1);
  lcd.print("Pontuacao:");

  lcd.setCursor(11, 1);
  lcd.print(points);
}

bool TurnOnLed(int col, int time)
{

  for (int i = 0; i < linhas; i++)
  {
    for (int j = 0; j < colunas; j++)
    {
      digitalWrite(LEDSPOS[j], (LEDSPOS[j] == LEDSPOS[col]));
    }

    for (int k = 0; k < linhas; k++)
    {
      digitalWrite(LEDSNEG[k], (LEDSNEG[k] != LEDSNEG[i]));
    }

    delay(time);

    if (i == (linhas - 1) && digitalRead(BUTTONS[col]) == HIGH)
    {
      points += 100;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DEUS DO RITMO");

      lcd.setCursor(0, 1);
      lcd.print("Pontuacao: +100");

      delay(500);

      printDefault();

      break;
    }

    else if (i != (linhas - 1) && digitalRead(BUTTONS[col]) == HIGH)
    {
      if (points <= 0)
      {
        points = 0;
      }
      else
      {
        points -= 50;
      }

      fails++;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ta adiantado ai?");

      lcd.setCursor(0, 1);
      lcd.print("Pontuacao: -50");

      delay(500);

      printDefault();

      break;
    }

    else if (fails == 5)
    {
      points = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAME OVER!");

      delay(1000);
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Aperte um botao");
      lcd.setCursor(0, 1);
      lcd.print("para reiniciar");

      while (1)
      {
        for (int i = 0; i < colunas; i++)
        {
          if (digitalRead(BUTTONS[i]) == HIGH)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Carregando...");
            count = 0;
            delay(1000);

            printDefault();

            return 0;
          }
        }
      }
    }
  }
  return 1;
}

void setup()
{
  // Iniciando LCD
  lcd.begin();
  lcd.backlight();

  // Iniciando Serial
  Serial.begin(115200);

  // pinMode Negativos
  for (int i = 0; i < linhas; i++)
  {
    pinMode(LEDSNEG[i], OUTPUT);
  }

  // pinMode Positivos e Botões
  for (int i = 0; i < colunas; i++)
  {
    pinMode(LEDSPOS[i], OUTPUT);
    pinMode(BUTTONS[i], INPUT_PULLDOWN);
  }

  /*
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
  */
}

void loop()
{
  while (count == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GUITAR HERO");
    lcd.setCursor(0, 1);
    lcd.print("Pressione para jogar");
    delay(2000);

    if (digitalRead(BUTTONS[0]) == HIGH)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GUITAR HERO");
      count = 1;
      break;
    }
  }

  while (count == 1)
  {
    if (TurnOnLed(0, 500) == 0)
    {
      return;
    }

    if (TurnOnLed(1, 500) == 0)
    {
      return;
    }

    if (TurnOnLed(2, 450) == 0)
    {
      return;
    }


    if (TurnOnLed(4, 400) == 0)
    {
      return;
    }

    if (TurnOnLed(2, 350) == 0)
    {
      return;
    }

    if (TurnOnLed(2, 200) == 0)
    {
      return;
    }

    if (TurnOnLed(4, 300) == 0)
    {
      return;
    }

    if (TurnOnLed(0, 200) == 0)
    {
      return;
    }

    count = 2;
    break;
  }

  if (count == 2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WHAT A SHOW!");
    lcd.setCursor(0, 1);
    lcd.print("Pontuacao:");
    lcd.setCursor(11, 1);
    lcd.print(points);

    delay(5000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Aperte um botao para");
    lcd.setCursor(0, 1);
    lcd.print("jogar novamente");

    while (1)
    {
      for (int i = 0; i < 2; i++)
      {
        if (digitalRead(BUTTONS[i]) == HIGH)
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Carregando...");
          count = 0;
          delay(1000);
          lcd.clear();
          return;
        }
      }
    }
  }
}