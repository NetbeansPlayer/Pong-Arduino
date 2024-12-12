#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define UP_BUTTON 2
#define DOWN_BUTTON 3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PADDLE_RATE 33
#define BALL_RATE 20
#define PADDLE_HEIGHT 24
#define MAX_SCORE 8

int CPU_SCORE = 0;
int PLAYER_SCORE = 0;

// Color 
#define BLACK 0
#define WHITE 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void drawCourt();

uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;

boolean gameIsRunning = true;
boolean resetBall = false;

const uint8_t CPU_X = 12;
uint8_t cpu_y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

unsigned long ball_update = 0;
unsigned long paddle_update = 0;

// Imagen de inicio


void setup() {
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Mostrar imagen de inicio
 

  display.clearDisplay();
  drawCourt();
  display.display();
  delay(1000);

  unsigned long start = millis();
  while (millis() - start < 2000);

  ball_update = millis();
  paddle_update = ball_update;
  ball_x = random(25, 65);
  ball_y = random(3, 63);
}

void loop() {
  unsigned long time = millis();
  static bool up_state = false;
  static bool down_state = false;

  up_state |= (digitalRead(UP_BUTTON) == LOW);
  down_state |= (digitalRead(DOWN_BUTTON) == LOW);

  if (resetBall) {
    ball_x = random(50, 70);
    ball_y = random(30, 63);
    do {
      ball_dir_x = random(-1, 2);
    } while (ball_dir_x == 0);

    do {
      ball_dir_y = random(-1, 2);
    } while (ball_dir_y == 0);

    resetBall = false;
  }

  if (time > ball_update && gameIsRunning) {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    if (new_x == 0) { // Player obtener punto
      PLAYER_SCORE++;
      if (PLAYER_SCORE == MAX_SCORE) {
        gameOver();
      } else {
        showScore();
      }
    }

    if (new_x == SCREEN_WIDTH - 1) { // CPU obtener punto
      CPU_SCORE++;
      if (CPU_SCORE == MAX_SCORE) {
        gameOver();
      } else {
        showScore();
      }
    }

    if (new_y == 0 || new_y == SCREEN_HEIGHT - 1) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    if (new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    display.drawPixel(ball_x, ball_y, BLACK);
    display.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;
    display.display();
  }

  if (time > paddle_update && gameIsRunning) {
    paddle_update += PADDLE_RATE;

    // CPU paddle
    display.fillRect(CPU_X, cpu_y, 1, PADDLE_HEIGHT, BLACK);
    const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
    if (cpu_y + half_paddle > ball_y) {
      cpu_y -= 1;
    }
    if (cpu_y + half_paddle < ball_y) {
      cpu_y += 1;
    }
    if (cpu_y < 1) cpu_y = 1;
    if (cpu_y + PADDLE_HEIGHT > SCREEN_HEIGHT - 1) cpu_y = SCREEN_HEIGHT - 1 - PADDLE_HEIGHT;
    display.fillRect(CPU_X, cpu_y, 1, PADDLE_HEIGHT, WHITE);

    // Player paddle
    display.fillRect(PLAYER_X, player_y, 1, PADDLE_HEIGHT, BLACK);
    if (up_state) {
      player_y -= 1;
    }
    if (down_state) {
      player_y += 1;
    }
    up_state = down_state = false;
    if (player_y < 1) player_y = 1;
    if (player_y + PADDLE_HEIGHT > SCREEN_HEIGHT - 1) player_y = SCREEN_HEIGHT - 1 - PADDLE_HEIGHT;
    display.fillRect(PLAYER_X, player_y, 1, PADDLE_HEIGHT, WHITE);

    display.display();
  }
}

void drawCourt() {
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
}

void gameOver() {
  gameIsRunning = false;
  display.clearDisplay();
  drawCourt();

  if (PLAYER_SCORE > CPU_SCORE) {
    display.setTextSize(2);
    display.setCursor(20, 4);
    display.println("You Won");
  } else {
    display.setTextSize(2);
    display.setCursor(20, 4);
    display.println("CPU Won");
  }

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(40, 30);
  display.println(String(CPU_SCORE));

  display.setTextColor(WHITE);
  display.setCursor(80, 30);
  display.println(String(PLAYER_SCORE));

  display.display();
  delay(2000);

  while (digitalRead(UP_BUTTON) == HIGH && digitalRead(DOWN_BUTTON) == HIGH) {
    delay(100);
  }
  gameIsRunning = true;

  CPU_SCORE = PLAYER_SCORE = 0;

  unsigned long start = millis();
  display.clearDisplay();
  drawCourt();
  display.display();
  while (millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;
  gameIsRunning = true;
  resetBall = true;
}

void showScore() {
  gameIsRunning = false;
  display.clearDisplay();
  drawCourt();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 4);
  display.println("Score");

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(40, 30);
  display.println(String(CPU_SCORE));

  display.setTextColor(WHITE);
  display.setCursor(80, 30);
  display.println(String(PLAYER_SCORE));

  display.display();
  delay(2000);

  unsigned long start = millis();
  display.clearDisplay();
  drawCourt();
  display.display();
  while (millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;
  gameIsRunning = true;
  resetBall = true;
}