#include <HCSR04.h>

// == Pinos ponte H ==
#define IN1 4
#define IN2 5
// == Pinos para o sensor ultrassônico ==
#define TRIG_PIN 6
#define ECHO_PIN 7
// == Pinos para os sensores infravermelhos ==
#define INFRARED_FRONT_PIN 13
#define INFRARED_BACK_PIN 12

// == Parâmetros ==
#define opponentDistante 30
#define timeRead 50
#define measureDistance 5

// == Variáveis para os sensores infravermelhos ==
int infraredFront;
int infraredBack;
// == Variáveis para execução ==
boolean opponent = false;
boolean stopBot = false;
boolean newRead;
boolean mediaSensor[measureDistance];
byte position;
byte opponentCount = 0;
unsigned long controlRead;

// == Instância do objeto sensor ultrassônico ==
HCSR04 sensorHCSR04(TRIG_PIN, ECHO_PIN);

void setup()
{
  // == Pinos da ponte H ==
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  // == Pinos do sensor infravermelho ==
  pinMode(INFRARED_FRONT_PIN, INPUT);
  pinMode(INFRARED_BACK_PIN, INPUT);

  // == Configurações do sensor ultrassônico ==
  for (byte i = 0; i < measureDistance; i++)
  {
    mediaSensor[i] = 0;
  }
  position = 0;

  // == Define a velocidade do motor entre 200/255 ==
  Serial.begin(9600);
  Serial.println("Iniciado com sucesso...");
}

void loop()
{
  // == Define os valores das variáveis do sensor infravermelho ==
  infraredFront = digitalRead(INFRARED_FRONT_PIN);
  infraredBack = digitalRead(INFRARED_BACK_PIN);

  // == Se o sensor infravermelho da FRENTE detectar a borda branca, deve ir para TRÁS ==
  if (infraredFront == 0 && infraredBack == 0)
  {
    goBackward();
  }
  // == Se o sensor infravermelho de TRÁS detectar a borda branca, deve ir para FRENTE ==
  else if (infraredFront == 1 && infraredBack == 1)
  {
    goForward();
  }
  // == Se os dois sensores infravermelhos detectarem a borda branca, deve PARAR ==
  else if (infraredFront == 1 && infraredBack == 0)
  {
    stop();
  }
  // == Calcula a distância entre o sensor ultrassonico e o oponente ==
  else if (infraredFront == 0 && infraredBack == 1)
  {
    // == Configurações do sensor ultrassônico ==
    newRead = true;

    if (millis() - controlRead > timeRead)
    {
      if (sensorHCSR04.dist() <= opponentDistante)
      {
        opponent = true;
        newRead = true;
      }
      else
      {
        opponent = false;
        newRead = true;
      }
      controlRead = millis();
    }

    if (newRead == true)
    {
      mediaSensor[position] = opponent;
      position = position + 1;

      for (byte i = 0; i < measureDistance; i++)
      {
        if (mediaSensor[i] == 1)
        {
          opponentCount++;
        }
      }

      if (opponentCount >= ((measureDistance / 2) + 1))
      {
        goForward();
      }
      else
      {
        stop();
      }

      opponentCount = 0;
      if (position > measureDistance)
      {
        position = 0;
      }
    }
  }

  // == Imprime as saídas
  Serial.println("=== LOGS ===");
  Serial.print("Sensor infravermelho frente: ");
  Serial.println(infraredFront);
  Serial.print("Sensor infravermelho atrás: ");
  Serial.println(infraredBack);

  delay(200);
}

// == Vai para frente ==
void goBackward()
{
  stop();
  delay(50);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Vamos para trás!");
}

// == Vai para trás ==
void goForward()
{
  stop();
  delay(50);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Vamos para frente!");
}

// == Para ==
void stop()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  Serial.println("Parando!");
}