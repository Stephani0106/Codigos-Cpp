#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include "uRTCLib.h"

#define LED_VERMELHO 7
#define LED_AMARELO 6
#define LED_VERDE 5
#define HIGROMETRO A0
#define SDA A4
#define SCL A5

int absoluteMinUmid = 0;
int absoluteMidUmid = 10;
int absoluteHighUmid = 14;
int absoluteMaxUmid = 20;

int umidade = 0;
int umidMax = 0;
int umidMin = 99;

const byte SCREEN_WIDTH = 128;
const byte SCREEN_HEIGHT = 64;

const int OLED_RESET = -1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
uRTCLib rtc(0x68);

int stepAtual = 0;
int stepDesejada = 0;

const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup()
{
    // Setup do display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 28);
    display.println("Iniciando Sistema");
    display.display();

    // Setup do Clock
    Serial.begin(9600);
    URTCLIB_WIRE.begin();

    // Setup dos pinos
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(HIGROMETRO, OUTPUT);
    pinMode(SDA, OUTPUT);
    pinMode(SCL, OUTPUT);

    // Setup do motor
    myStepper.setSpeed(60);

    Serial.println("Iniciado com sucesso...");
}

void loop()
{
    delay(500);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERDE, LOW);

    rtc.refresh();
    int umidadeLida = analogRead(HIGROMETRO);
    umidade = umidadeLida;

    if (umidadeLida > umidMax)
    {
        umidMax = umidadeLida;
    }
    else if (umidadeLida < umidMin)
    {
        umidMin = umidadeLida;
    }

    if (umidade >= absoluteHighUmid)
    {
        // Liga o LED verde
        piscaVerde(umidade, umidMax, umidMin);
        stepDesejada = 200;
    }
    else if (umidade > absoluteMidUmid && umidade < absoluteHighUmid)
    {
        // Liga o LED amarelo
        piscaAmarelo(umidade, umidMax, umidMin);
        stepDesejada = 100;
    }
    else
    {
        // Liga o LED vermelho
        piscaVermelho(umidade, umidMax, umidMin);
        stepDesejada = 0;
    }
    exibeUmidade(umidade, umidMax, umidMin, rtc.day(), rtc.month());
    giraStepper(stepDesejada, stepAtual);

    // == Imprime as saídas
    Serial.println("=== LOGS ===");
    Serial.print("Umidade umidade: ");
    Serial.print((String)umidade + " | ");
    Serial.println((String)map(umidade, absoluteMinUmid, absoluteMaxUmid, 0, 100) + "%");
    Serial.print("Umidade maxima: ");
    Serial.print((String)umidMax + " | ");
    Serial.println((String)map(umidMax, absoluteMinUmid, absoluteMaxUmid, 0, 100) + "%");
    Serial.print("Umidade minima: ");
    Serial.print((String)umidMin + " | ");
    Serial.println((String)map(umidMin, absoluteMinUmid, absoluteMaxUmid, 0, 100) + "%");
}

void giraStepper(int desejada, int atual)
{
    myStepper.step(desejada - atual);
    stepAtual = stepDesejada;
    delay(500);
    display.clearDisplay();
}

void piscaVermelho(int umidade, int umidMax, int umidMin)
{
    delay(500);
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERDE, LOW);

    Serial.println("Muito úmido!");
}

void piscaAmarelo(int umidade, int umidMax, int umidMin)
{
    delay(500);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_AMARELO, HIGH);
    digitalWrite(LED_VERDE, LOW);

    Serial.println("Meio úmido.");
}

void piscaVerde(int umidade, int umidMax, int umidMin)
{
    delay(500);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERDE, HIGH);

    Serial.println("Não está úmido.");
}

void exibeUmidade(int umidade, int umidMax, int umidMin, int dia, int mes)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Umidade atual:");
    display.setCursor(14, 0);
    display.print(map(umidade, absoluteMinUmid, absoluteMaxUmid, 0, 100));
    display.print("%");

    display.setCursor(0, 8);
    display.println("Umidade maxima:");
    display.setCursor(15, 8);
    display.print(map(umidMax, absoluteMinUmid, absoluteMaxUmid, 0, 100));
    display.print("%");

    display.setCursor(0, 16);
    display.println("Umidade minima:");
    display.setCursor(15, 16);
    display.print(map(umidMin, absoluteMinUmid, absoluteMaxUmid, 0, 100));
    display.print("%");
    display.drawFastHLine(0, 26, SCREEN_WIDTH, 1);

    display.setCursor(0, 34);
    display.println("Data: ");
    display.print(dia);
    display.print("/");
    display.print(mes);
    display.print("/");
    display.print(2023);

    display.display();
}