
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

// ================================
// PROJETO A3 - ILUMINACAO INTELIGENTE
// Arduino UNO + BH1750 + LCD I2C
// MOSFET IRLZ44N + LED
// ================================

BH1750 sensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinoLED = 9;

// Meta experimental de iluminancia
const float setpointLux = 300.0;

// Faixa de controle proporcional
const float faixaControleLux = 300.0;

float lux = 0.0;
int pwm = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(pinoLED, OUTPUT);
  analogWrite(pinoLED, 0);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Sistema de Luz");

  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  delay(1500);

  if (!sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro BH1750");

    Serial.println("ERRO: BH1750 nao encontrado.");

    while (true) {
      analogWrite(pinoLED, 0);
      delay(100);
    }
  }

  lcd.clear();
  Serial.println("Sistema iniciado.");
}

void loop() {
  lux = sensor.readLightLevel();

  // Verifica se a leitura e valida
  if (lux < 0) {
    analogWrite(pinoLED, 0);

    lcd.setCursor(0, 0);
    lcd.print("Erro de leitura ");

    lcd.setCursor(0, 1);
    lcd.print("LED desligado   ");

    Serial.println("ERRO: leitura do BH1750.");
    delay(500);
    return;
  }

  // Calcula a diferenca para a meta
  float faltaLux = setpointLux - lux;

  // Converte a diferenca em PWM
  pwm = (int)(255.0 * faltaLux / faixaControleLux);

  // Limita o PWM entre 0 e 255
  pwm = constrain(pwm, 0, 255);

  // Aciona o MOSFET
  analogWrite(pinoLED, pwm);

  // Exibe a iluminancia no LCD
  lcd.setCursor(0, 0);
  lcd.print("Luz: ");
  lcd.print(lux, 1);
  lcd.print(" lx    ");

  // Exibe o PWM no LCD
  lcd.setCursor(0, 1);
  lcd.print("PWM: ");
  lcd.print(pwm);
  lcd.print("          ");

  // Envia os dados pela serial
  Serial.print("Lux: ");
  Serial.print(lux, 1);
  Serial.print(" | PWM: ");
  Serial.println(pwm);

  delay(500);
}
