
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

// Sensor e display
BH1750 sensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuracoes
const int pinoLED = 9;
const float setpointLux = 300.0;
const float faixaControleLux = 300.0;

// Variaveis
float lux = 0.0;
int pwm = 0;

void setup() {

  Serial.begin(9600);
  Wire.begin();

  // Configuracao do PWM
  pinMode(pinoLED, OUTPUT);
  analogWrite(pinoLED, 0);

  // Inicializacao do LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Sistema de Luz");

  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  delay(1500);

  // Inicializacao do BH1750
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
  Serial.println("Lux,PWM");
}

void loop() {

  // Leitura do sensor
  lux = sensor.readLightLevel();

  // Verificacao de erro
  if (lux < 0) {

    analogWrite(pinoLED, 0);

    lcd.setCursor(0, 0);
    lcd.print("Erro de leitura");

    lcd.setCursor(0, 1);
    lcd.print("LED desligado");

    Serial.println("ERRO: leitura BH1750");

    delay(500);
    return;
  }

  // Calculo da iluminancia faltante
  float faltaLux = setpointLux - lux;

  // Controle proporcional
  pwm = (int)(255.0 * faltaLux / faixaControleLux);

  // Limitacao do PWM
  pwm = constrain(pwm, 0, 255);

  // Acionamento do LED
  analogWrite(pinoLED, pwm);

  // Exibicao no LCD
  lcd.setCursor(0, 0);
  lcd.print("Lux: ");
  lcd.print(lux, 1);
  lcd.print("        ");

  lcd.setCursor(0, 1);
  lcd.print("PWM: ");
  lcd.print(pwm);
  lcd.print("          ");

  // Comunicacao serial com Python
  Serial.print(lux, 1);
  Serial.print(",");
  Serial.println(pwm);

  delay(500);
}
