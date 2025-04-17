const int ledPins[3] = {3, 5, 6};
const int buttonPins[3] = {7, 8, 9};
const int successLed = 10;
const int buzzerPin = 11;
const int ldrPin = A0;

int sequence[3];
int userInput[3];
int brightness = 0;
unsigned long startTime;
unsigned long timeout = 5000;
bool gameStarted = false;
int inputCount = 0;

// Bah comme d'hab on configure les broches et démarre le jeu au lancement
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(successLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  startGame();
}

// la boucle principale du jeu, vérifie le temps et lit les entrées
void loop() {
  if (gameStarted && millis() - startTime > timeout) {
    fail("Le sable du temps a scellé votre destin, vous n'êtes pas digne des runes !");
    delay(1000);
    startGame();
    return;
  }
  
  if (gameStarted) {
    readUserInput();
  }
}

// ajuste la luminosité des LEDs en fonction de la lumière ambiante
void adjustBrightness() {
  int ldrValue = analogRead(ldrPin);
  brightness = map(ldrValue, 0, 1023, 255, 50);
}

// initialise une nouvelle partie en générant et affichant une séquence
void startGame() {
  digitalWrite(successLed, LOW);
  generateSequence();
  displaySequence();
  startTime = millis();
  inputCount = 0;
  gameStarted = true;
  Serial.println("Les runes brillent dans l'obscurité, reproduisez leur éclat avant que le temple ne vous engloutisse (5 secondes) !");
}

// génère une séquence aléatoire pour les LEDs
void generateSequence() {
  int temp[3] = {0, 1, 2};
  for (int i = 2; i > 0; i--) {
    int j = random(i + 1);
    int swap = temp[i];
    temp[i] = temp[j];
    temp[j] = swap;
  }
  for (int i = 0; i < 3; i++) sequence[i] = temp[i];
}

// Affiche la séquence générée avec les LEDs
void displaySequence() {
  for (int i = 0; i < 3; i++) {
    adjustBrightness();
    analogWrite(ledPins[sequence[i]], brightness);
    delay(500);
    analogWrite(ledPins[sequence[i]], 0);
    delay(300);
  }
}

// Lit les entrées du joueur et vérifie la séquence
void readUserInput() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      Serial.print("Rune ");
      Serial.print(inputCount + 1);
      Serial.print(" invoquée → Artefact ");
      Serial.println(i + 1);
      userInput[inputCount] = i;
      adjustBrightness();
      analogWrite(ledPins[i], brightness);
      delay(300);
      analogWrite(ledPins[i], 0);
      delay(300);
      if (userInput[inputCount] != sequence[inputCount]) {
        fail("Les runes rejettent votre offrande, le temple tremble de votre erreur !");
        delay(1000);
        startGame();
        return;
      }
      inputCount++;
      if (inputCount == 3) {
        success();
        gameStarted = false;
        delay(3000);
        Serial.println("La crypte s'ouvre, mais d'autres secrets vous attendent. Touchez un artefact pour défier à nouveau les runes !");
        waitForStart();
        return;
      }
      while (digitalRead(buttonPins[i]) == LOW);
    }
  }
}

// Signale la victoire avec la LED et une mélodie
void success() {
  digitalWrite(successLed, HIGH);
  playOdeToJoy();
}

// Signale un échec avec un message et un son
void fail(const char* reason) {
  Serial.print("Les anciens murmurent : ");
  Serial.println(reason);
  tone(buzzerPin, 200, 1000);
  delay(1000);
}

// Attend une pression de bouton pour relancer le jeu
void waitForStart() {
  while (true) {
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(300);
        startGame();
        return;
      }
    }
  }
}

// Joue une mélodie de victoire avec le buzzer
void playOdeToJoy() {
  int melody[] = {262, 262, 294, 330, 330, 294, 262, 247, 220, 220, 247, 262, 262, 247, 247};
  int noteDurations[] = {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 450, 450};
  for (int i = 0; i < 15; i++) {
    tone(buzzerPin, melody[i], noteDurations[i]);
    delay(noteDurations[i] + 50);
  }
}