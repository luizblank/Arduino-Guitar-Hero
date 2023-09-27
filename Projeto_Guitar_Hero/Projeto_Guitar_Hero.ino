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

int centralIndex(char linha[])
{
    int tamanholinha = 0;
    for (int i = 0; linha[i] != '\0'; i++)
    {
        tamanholinha++;
    }

    int x = ((SIZE_LCD - tamanholinha) - 1 / 2) - 1;

    Serial.print("Tamanho linha:");
    Serial.println(tamanholinha);
    Serial.print("Coordenada X:");
    Serial.println(x);
    Serial.println();
    return x;
}

void printLCD(char linha1[], char linha2[], bool x1 = false, bool x2 = false)
{
    int xcentral1 = 0;
    int xcentral2 = 0;

    int tamanholinha1 = 0;
    int tamanholinha2 = 0;

    for (int i = 0; linha1[i] != '\0'; i++)
    {
        tamanholinha1++;
    }

    for (int i = 0; linha2[i] != '\0'; i++)
    {
        tamanholinha2++;
    }

    Serial.print("sizeof(linha1): ");
    Serial.println(tamanholinha1);
    Serial.print("sizeof(linha2): ");
    Serial.println(tamanholinha2);
    Serial.println();

    if (x1)
    {
        xcentral1 = centralIndex(linha1);
    }

    if (x2)
    {
        xcentral2 = centralIndex(linha2);
    }

    lcd.clear();

    for (int i = 0; i < tamanholinha1; i++)
    {
        lcd.setCursor(i + xcentral1, 0);
        lcd.print(linha1[i]);
    }

    for (int i = 0; i < tamanholinha2; i++)
    {
        lcd.setCursor(i + xcentral2, 1);
        lcd.print(linha2[i]);
    }
}

void printDefault()
{
    printLCD("GUITAR HERO", "Pontuacao:", true, false);
    lcd.setCursor(11, 1);
    lcd.print(points);
}

void printRestart()
{
    printLCD("Aperte um botao", "para reiniciar");
}

void resetAndPrintLoad()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Carregando...");
    count = 0;
    points = 0;
    fails = 0;
    delay(1000);

    printDefault();
}

bool verifyCorrect(int8_t notas)
{
    for (int i = 0; i < colunas; i++)
    {
        int8_t op = 1 << i;
        bool logica = notas && op > 0;
        if (digitalRead(LEDSPOS[i]) != logica)
            return false;
    }

    return true;
}

bool TurnOnLed(int8_t notas, int time)
{
    for (int i = 0; i < linhas; i++)
    {
        // Controla qual entrada (colunas) vai passar energia positiva constante
        for (int j = 0; j < colunas; j++)
        {
            int8_t op = 1 << i;
            bool logica = notas && op > 0;
            digitalWrite(LEDSPOS[j], (LEDSPOS[j] == logica));
        }

        // Controla qual linha que vai ficar acesa e quais vão ficar apagadas
        for (int k = 0; k < linhas; k++)
            digitalWrite(LEDSNEG[k], (LEDSNEG[k] != LEDSNEG[i]));

        delay(time);

        int realindex = linhas - 1;
        bool verificacao = verifyCorrect(notas);

        if (i == realindex && verificacao == true)
        {
            points += 100;

            printLCD("DEUS DO RITMO!", "Pontuacao: +100", true, false);
            delay(500);
            printDefault();

            break;
        }

        else if (i != realindex && verificacao == false)
        {
            if ((points -= 50) <= 0)
                points = 0;
            else
                points -= 50;

            fails += 1;

            printLCD("Ta adiantado ai?", "Pontuacao: -50", true, false);
            delay(500);
            printDefault();

            break;
        }

        else if (fails == 5)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("GAME OVER!");
            delay(1000);
            printRestart();

            while (1)
            {
                for (int i = 0; i < colunas; i++)
                    if (digitalRead(BUTTONS[i]) == HIGH)
                    {
                        resetAndPrintLoad();
                        return 0;
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
    // char guitar[] = "GUITAR";
    // char hero[] = "HERO";
    // printLCD(guitar, hero);
    while (count == 0)
    {
        printLCD("Guitar", "Hero", true, true);
        delay(2000);
        printLCD("Pressione para", "jogar...");
        delay(2000);

        if (digitalRead(BUTTONS[0]) == HIGH)
        {
            count = 1;
            break;
        }
    }

    printLCD("Carregando...", "");
    delay(1000);
    printLCD("3", "");
    delay(1000);
    printLCD("2", "");
    delay(1000);
    printLCD("1", "");
    delay(1000);

    while (count == 1)
    {
        if (TurnOnLed(0b01010, 500) == 0)
        {
            return;
        }
        if (TurnOnLed(0b10101, 300) == 0)
        {
            return;
        }
        if (TurnOnLed(0b10000, 500) == 0)
        {
            return;
        }
        if (TurnOnLed(0b00101, 300) == 0)
        {
            return;
        }

        count = 2;
        break;
    }

    delay(2000);

    if (count == 2)
    {
        printLCD("WHAT A SHOW!", "Pontuacao:");
        lcd.setCursor(11, 1);
        lcd.print(points);
        delay(5000);
        printRestart();

        while (1)
        {
            if (digitalRead(BUTTONS[0]) == HIGH)
            {
                resetAndPrintLoad();
                return;
            }
        }
    }
}
