#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TEMP_SENSOR 2
#define IN1 12
#define IN2 4
#define POT_PIN A0

//variavel auxiliar
int potenciometro = 0;

OneWire oneWire(TEMP_SENSOR);        /*Protocolo OneWire*/
DallasTemperature sensors(&oneWire); /*encaminha referências OneWire para o sensor*/

const int OLED_RESET = -1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int tempDesejada = 20;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 20, Ki = 2, Kd = 0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Setup do display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println("Iniciando Sistema");
  display.display();

  sensors.begin(); /*inicia biblioteca*/

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN2, LOW);

  //initialize the variables we're linked to
  Input = readTemperature();
  Setpoint = tempDesejada;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  analogWrite(IN2, 255);
  delay(1500);
}

void loop() {
  displayTemperature(readTemperature(), tempDesejada);

  potenciometro = analogRead(POT_PIN);
  tempDesejada = map(potenciometro, 0, 1023, 20, 50);

  Setpoint = tempDesejada;
  Input = readTemperature();
  myPID.Compute();
  int convertOut = constrain((int)Output, 80, 255);
  analogWrite(IN1, convertOut);

  Serial.print("PID: ");
  Serial.println(convertOut);
  Serial.println("==================");
}

int readTemperature() {  // Lê a temperatura
  sensors.requestTemperatures();
  int y = (int)sensors.getTempCByIndex(0);
  Serial.print("A temperatura é: ");          /* Printa "A temperatura é:" */
  Serial.println(y);                          /* Endereço do sensor */
  Serial.println(sensors.getTempCByIndex(0)); /* Endereço do sensor */

  return y;
}

void displayTemperature(int tempAtual, int tempDesejada) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 8);
  display.print("Temp atual:");
  display.print(tempAtual);
  display.print(" C");
  display.display();

  display.drawFastHLine(0, 26, SCREEN_WIDTH, 1);

  display.setTextSize(1);
  display.setCursor(0, 34);
  display.print("Temp desejada:");
  display.print(tempDesejada);
  display.print(" C");
  display.display();

  delay(50);
}