#include <Servo.h>

Servo servo;

int pinoServo = 6;
int trig = 9;
int echo = 10;

int buzzer = 3;

int ledVermelho = 5;
int ledVerde = 7;

int distanciaAlvo = 25;

int anguloMin = 30;
int anguloMax = 150;
int angulo = 90;
int direcao = 1;
int passoServo = 2;

unsigned long ultimoMovServo = 0;
unsigned long intervaloServo = 25;

bool preAlarme = false;
bool alarmeAtivo = false;

unsigned long ultimoVisto = 0;
unsigned long inicioPreAlarme = 0;

int tomPre = 3200;
unsigned long ultimoBip = 0;
unsigned long intervaloBip = 600;

int tipoSirene = 1;
unsigned long trocaSirene = 0;
unsigned long ultimoSom = 0;

int freq = 2600;
bool sobe = true;

long medirDistancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long tempo = pulseIn(echo, HIGH, 25000);
  if (tempo == 0) return -1;
  return tempo * 0.034 / 2;
}

void setup() {
  servo.attach(pinoServo);
  servo.write(angulo);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledVermelho, LOW);
  noTone(buzzer);
}

void loop() {
  unsigned long agora = millis();
  long d = medirDistancia();

  if (d > 0 && d < distanciaAlvo) {
    ultimoVisto = agora;

    if (!preAlarme && !alarmeAtivo) {
      preAlarme = true;
      inicioPreAlarme = agora;
      tomPre = 3200;
      intervaloBip = 600;
    }
  }

  if (preAlarme || alarmeAtivo) {
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);
  } else {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);
  }

  if (preAlarme) {
    if (agora - ultimoBip > intervaloBip) {
      tone(buzzer, tomPre);
      delay(60);
      noTone(buzzer);

      if (tomPre > 900) tomPre -= 200;
      if (intervaloBip > 120) intervaloBip -= 60;

      ultimoBip = agora;
    }

    if (agora - inicioPreAlarme > 4000) {
      preAlarme = false;
      alarmeAtivo = true;
      tipoSirene = 1;
      trocaSirene = agora;
      freq = 2600;
      sobe = true;
    }
  }

  if (alarmeAtivo) {

    if (agora - trocaSirene > 5000) {
      tipoSirene++;
      if (tipoSirene > 3) tipoSirene = 1;
      trocaSirene = agora;
    }

    if (tipoSirene == 1 && agora - ultimoSom > 40) {
      freq += sobe ? 140 : -140;
      if (freq >= 3800) sobe = false;
      if (freq <= 2200) sobe = true;
      tone(buzzer, freq);
      ultimoSom = agora;
    }

    if (tipoSirene == 2 && agora - ultimoSom > 120) {
      tone(buzzer, 3500);
      delay(20);
      tone(buzzer, 900);
      ultimoSom = agora;
    }

    if (tipoSirene == 3 && agora - ultimoSom > 80) {
      static bool alto = true;
      tone(buzzer, alto ? 3600 : 2400);
      alto = !alto;
      ultimoSom = agora;
    }
  }

  if (!preAlarme && !alarmeAtivo) {
    if (agora - ultimoMovServo > intervaloServo) {
      angulo += direcao * passoServo;
      if (angulo >= anguloMax) direcao = -1;
      if (angulo <= anguloMin) direcao = 1;
      servo.write(angulo);
      ultimoMovServo = agora;
    }
  }

  if ((preAlarme || alarmeAtivo) && agora - ultimoVisto > 1500) {
    preAlarme = false;
    alarmeAtivo = false;
    noTone(buzzer);
  }
}
