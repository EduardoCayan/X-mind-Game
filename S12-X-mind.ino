#include <LiquidCrystal.h>
#include <EEPROM.h>

// Configuración de pines LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Pines y variables del juego
const int buttonPin = 8;
const int buzzerPin = 9;  // Buzzer conectado aquí
unsigned long previousMillis = 0;
long interval = 2000;
int targetPosition = 0;
int score = 0;
int highScore = 0;
bool gameActive = false;
bool targetVisible = false;

// Dirección EEPROM para guardar el récord
const int highScoreAddr = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  
  // Leer récord de la EEPROM
  highScore = EEPROM.read(highScoreAddr);
  
  // Si es 255 (valor inicial), resetear a 0
  if (highScore == 255) {
    highScore = 0;
    EEPROM.write(highScoreAddr, 0);
  }
  
  // Pantalla inicial con sonido de prueba
  lcd.print("  JUEGA YA!  ");
  lcd.setCursor(0, 1);
  lcd.print("Record: ");
  lcd.print(highScore);
  playStartupSound();  // Sonido de inicio
}

void loop() {
  if (!gameActive) {
    if (digitalRead(buttonPin) == LOW) {
      startGame();
    }
    return;
  }

  unsigned long currentMillis = millis();

  // Lógica de generación de objetivos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (targetVisible) {
      hideTarget();
      playMissSound();  // Sonido al fallar
      gameOver();
    } else {
      generateTarget();
    }
  }

  // Detectar presión del botón
  if (digitalRead(buttonPin) == LOW) {
    delay(50);  // Debounce
    
    if (targetVisible) {
      score++;
      hitTarget();
    } else {
      playMissSound();  // Sonido al presionar sin objetivo
      gameOver();
    }
  }
}

void startGame() {
  gameActive = true;
  score = 0;
  interval = 2000;
  lcd.clear();
  updateDisplay();
  playStartSound();  // Sonido de inicio de juego
  generateTarget();
}

void generateTarget() {
  targetPosition = random(0, 16);
  targetVisible = true;
  lcd.setCursor(targetPosition, 0);
  lcd.print("X");
  previousMillis = millis();
  
  // Sonido de aparición de objetivo
  tone(buzzerPin, 3520, 80);
}

void hideTarget() {
  targetVisible = false;
  lcd.setCursor(targetPosition, 0);
  lcd.print(" ");
}

void hitTarget() {
  hideTarget();
  interval = max(300, interval - 50);  // Aumenta dificultad
  playHitSound();  // Sonido de acierto
  updateDisplay();
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Puntos: ");
  lcd.print(score);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("Record: ");
  lcd.print(highScore);
  lcd.print("    ");
}

void gameOver() {
  gameActive = false;
  playGameOverSound();  // Sonido de fin de juego
  
  // Actualizar récord si es necesario
  if (score > highScore) {
    highScore = score;
    EEPROM.write(highScoreAddr, highScore);
    playNewRecordSound();  // Sonido de nuevo récord
  }
  
  lcd.clear();
  lcd.print("GAME OVER! Pts:");
  lcd.setCursor(0, 1);
  lcd.print(score);
  lcd.print(" Record: ");
  lcd.print(highScore);
  delay(3000);
  
  // Resetear pantalla
  lcd.clear();
  lcd.print("  JUEGA YA!  ");
  lcd.setCursor(0, 1);
  lcd.print("Record: ");
  lcd.print(highScore);
}

// Funciones de sonido mejoradas
void playStartupSound() {
  tone(buzzerPin, 523, 150); delay(150);  // Do
  tone(buzzerPin, 659, 150); delay(150);  // Mi
  tone(buzzerPin, 784, 300);              // Sol
  delay(300);
  noTone(buzzerPin);
}

void playStartSound() {
  tone(buzzerPin, 1047, 200);  // Do alto
  delay(200);
  noTone(buzzerPin);
}

void playHitSound() {
  tone(buzzerPin, 1318, 80);  // Mi alto
  delay(80);
  noTone(buzzerPin);
}

void playMissSound() {
  tone(buzzerPin, 220, 300);  // La bajo
  delay(300);
  noTone(buzzerPin);
}

void playGameOverSound() {
  tone(buzzerPin, 175, 500);  // Fa bajo
  delay(500);
  noTone(buzzerPin);
}

void playNewRecordSound() {
  tone(buzzerPin, 1568, 200); delay(200);  // Sol alto
  tone(buzzerPin, 2093, 400);              // Do súper alto
  delay(400);
  noTone(buzzerPin);
}