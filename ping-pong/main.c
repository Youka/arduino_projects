// ######## device_conf.h ########

#ifndef __DEVICE_CONF_H__
#define __DEVICE_CONF_H__

// Digital pins used (general)
enum DigitalPins{
  BUTTON_RIGHT2 = 8,
  BUTTON_LEFT2,
  BUTTON_RIGHT1,
  BUZZER,
  BUTTON_LEFT1
};
// Analog pins used
enum AnalogPins{
  POTENTIOMETER = 0
};
// Digital pins used on LCD
enum LCDPins{
  LCD_D7 = 2,
  LCD_D6,
  LCD_D5,
  LCD_D4,
  LCD_ENABLE,
  LCD_RS
};
// Dimensions of used LCD
#define LCD_WIDTH 16
#define LCD_HEIGHT 2
#define LCD_CHAR_WIDTH 5
#define LCD_CHAR_HEIGHT 8
#define LCD_PIXEL_WIDTH LCD_WIDTH*LCD_CHAR_WIDTH
#define LCD_PIXEL_HEIGHT LCD_HEIGHT*LCD_CHAR_HEIGHT
#define LCD_CUSTOM_CHARS_N 8

// Default serial bits-per-second
#define SERIAL_BAUD 9600

#endif

// ######## graphics.h ########

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

// Add dot to graphical back-buffer
bool g_add_dot(const byte x, const byte y);
// Add horizontal left-to-right line to graphical back-buffer
bool g_add_hline(char x, const char y, const byte len);
// Add vertical top-to-bottom line to graphical back-buffer
bool g_add_vline(const char x, char y, const byte len);

// Draw back-buffer
void g_draw(void);
// Draw text directly
void g_draw_text(const byte x, const byte y, const String s);
// Clear back-buffer and display
void g_clear(void);

#endif

// ######## graphics.c ########

//#include "graphics.h"
#include <LiquidCrystal.h>
//#include "device_conf.h"

// Output device & memory
static LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
static struct LCDInitializer{LCDInitializer(struct LiquidCrystal* lcd){lcd->begin(LCD_WIDTH, LCD_HEIGHT);}} _lcd(&lcd);
static byte chars[LCD_HEIGHT][LCD_WIDTH][LCD_CHAR_HEIGHT] = {{{0}}};

static bool char_dot_position(const byte x, const byte y, byte* char_x, byte* char_y, byte* inner_x, byte* inner_y){
  if(x >= LCD_PIXEL_WIDTH || y >= LCD_PIXEL_HEIGHT)
  	return false;
  const div_t div_x = div(x, LCD_CHAR_WIDTH),
  			div_y = div(y, LCD_CHAR_HEIGHT);
  *char_x = div_x.quot;
  *char_y = div_y.quot;
  *inner_x = div_x.rem;
  *inner_y = div_y.rem;
  return true;
}
bool g_add_dot(const byte x, const byte y){
  byte char_x, char_y, inner_x, inner_y;
  if(!char_dot_position(x, y, &char_x, &char_y, &inner_x, &inner_y))
    return false;
  chars[char_y][char_x][inner_y] |= bit(LCD_CHAR_WIDTH-1) >> inner_x;
  return true;
}
bool g_add_hline(char x, const char y, const byte len){
  if(y >= 0)
  	for(const char x2 = x+len-1; x <= x2; ++x)
      if(x >= 0)
      	g_add_dot(x, y);
}
bool g_add_vline(const char x, char y, const byte len){
  if(x >= 0)
  	for(const char y2 = y+len-1; y <= y2; ++y)
      if(y >= 0)
      	g_add_dot(x, y);
}


void g_draw(void){
  // Characters to draw storage
  byte n_chars = 0;
  struct {byte x, y;} chars_pos[LCD_CUSTOM_CHARS_N];
  // Collect characters to draw
  for(byte y = 0; y < sizeof(chars)/sizeof(chars[0]); ++y)
    for(byte x = 0; x < sizeof(chars[0])/sizeof(chars[0][0]); ++x){
      const byte* current_char = chars[y][x];
      for(byte row = 0; row < sizeof(chars[0][0])/sizeof(chars[0][0][0]); ++row)
        if(current_char[row]){
          lcd.createChar(n_chars, (byte*)current_char);
          chars_pos[n_chars].x = x;
          chars_pos[n_chars++].y = y;
          if(n_chars == sizeof(chars_pos)/sizeof(chars_pos[0])) // Limit reached
            goto draw_chars;
          break;
        }
    }
  // Draw characters
  draw_chars:
  for(byte c = 0; c < n_chars; ++c){
    lcd.setCursor(chars_pos[c].x, chars_pos[c].y);
    lcd.write(c);
  }
}
void g_draw_text(const byte x, const byte y, const String s){
  lcd.setCursor(x, y);
  lcd.print(s);
}
void g_clear(void){
  memset(chars, 0, sizeof(chars));
  lcd.clear();
}

// ######## init.c ########

//#include "device_conf.h"
//#include "graphics.h"

// Initialize Arduino device
void setup(){
  // Set pin modes
  pinMode(BUTTON_LEFT1, INPUT);
  pinMode(BUTTON_RIGHT1, INPUT);
  pinMode(BUTTON_LEFT2, INPUT);
  pinMode(BUTTON_RIGHT2, INPUT);
  pinMode(BUZZER, OUTPUT);
  // Initialize serial connection
  Serial.begin(SERIAL_BAUD);
  while(!Serial) // Wait for device response
    delay(100);
  // Show welcome text
  g_draw_text(0, 0, "Ping-Pong");
  delay(3000);
}

// ######## math.h ########

#ifndef __MATH_H__
#define __MATH_H__

// Ranged random numbers
#define RANDOM_DIRECTION (((random() & 0x1) << 1) - 1)
#define RANDOM_STEP(start, end, step) (start + random() % int((end-start) / step + 1) * step)
// Swap numbers
#define SWAP(a,b) (a ^= b ^= a ^= b)
void swapf(float* a, float* b);
// Collision detection
bool line_x_hline(float x1, float y1, float x2, float y2, const float lx, const float ly, const float llen, float* cx);
bool line_x_vline(float x1, float y1, float x2, float y2, const float lx, const float ly, const float llen, float* cy);

#endif

// ######## math.c ########

//#include "math.h"

void swapf(float* a, float* b){
  const float temp = *a;
  *a = *b;
  *b = temp;
}

bool line_x_hline(float x1, float y1, float x2, float y2, const float lx, const float ly, const float llen, float* cx){
  // Both lines horizontal / parallel?
  if(y1 == y2)
  	return false;
  // Sort line points top-to-bottom
  if(y1 > y2)
    swapf(&x1,&x2), swapf(&y1,&y2);
  // Lines can't cross vertical?
  if(ly < y1 || ly > y2)
    return false;
  // Lines crossing
  *cx = x1 == x2/*Vertical line?*/ ? x1 : (x2 - x1) / (y2 - y1) * (ly - y1) + x1;
  // Lines can cross horizontal?
  return *cx >= lx && *cx <= lx + llen;
}

bool line_x_vline(float x1, float y1, float x2, float y2, const float lx, const float ly, const float llen, float* cy){
  // Both lines vertical / parallel?
  if(x1 == x2)
  	return false;
  // Sort line points left-to-right
  if(x1 > x2)
    swapf(&x1,&x2), swapf(&y1,&y2);
  // Lines can't cross horizontal?
  if(lx < x1 || lx > x2)
    return false;
  // Lines crossing
  *cy = y1 == y2/*Horizontal line?*/ ? y1 : (y2 - y1) / (x2 - x1) * (lx - x1) + y1;
  // Lines can cross vertical?
  return *cy >= ly && *cy <= ly + llen;
}

// ######## main.c ########

//#include "device_conf.h"
//#include "graphics.h"
//#include "math.h"

// Game properties
#define PLAYER_SIZE 7
#define MIN_UPDATE_DELAY 100
#define MAX_UPDATE_DELAY 2000
#define SOUND_FREQUENCY 1000
#define SOUND_DURATION 50
#define PLAYER1_X 0
#define PLAYER2_X LCD_PIXEL_WIDTH-1
// Game state
static byte player1_pos = 0,
            player2_pos = 0;
static float ball_pos_x = LCD_PIXEL_WIDTH / 2.0f,
             ball_pos_y = LCD_PIXEL_HEIGHT / 2.0f,
             ball_mov_x = RANDOM_DIRECTION * RANDOM_STEP(1, 3, 0.5f),
             ball_mov_y = RANDOM_DIRECTION * RANDOM_STEP(1, 3, 0.5f);
static bool change_action = true;

// Collision logic
static void player_reflect(const byte area){
  ball_mov_x = -ball_mov_x;
  switch(area){	// Upper-, mid- or lower area of player
    case 0:
      if(ball_mov_y > 0)
        ball_mov_y /= 2, ball_mov_x *= 2;
      else if(ball_mov_y < 0)
        ball_mov_y *= 2, ball_mov_x /= 2;
      else
        ball_mov_y = -abs(ball_mov_x);
      break;
    case 1:
      break;
    case 2:
    default:
      if(ball_mov_y > 0)
        ball_mov_y *= 2, ball_mov_x /= 2;
      else if(ball_mov_y < 0)
        ball_mov_y /= 2, ball_mov_x *= 2;
      else
        ball_mov_y = abs(ball_mov_x);
      break;
  }
}

// Arduino device idle callback
void loop(){
  // Profile current run
  const unsigned long profile_start_time = millis();
  // Update player position
  const byte button_left1_pressed = digitalRead(BUTTON_LEFT1),
             button_right1_pressed = digitalRead(BUTTON_RIGHT1),
             button_left2_pressed = digitalRead(BUTTON_LEFT2),
             button_right2_pressed = digitalRead(BUTTON_RIGHT2);
  if(button_left1_pressed && !button_right1_pressed && player1_pos != 0)
    --player1_pos, change_action = true;
  else if(!button_left1_pressed && button_right1_pressed && player1_pos != LCD_PIXEL_HEIGHT-PLAYER_SIZE)
    ++player1_pos, change_action = true;
  if(button_left2_pressed && !button_right2_pressed && player2_pos != LCD_PIXEL_HEIGHT-PLAYER_SIZE)
    ++player2_pos, change_action = true;
  else if(!button_left2_pressed && button_right2_pressed && player2_pos != 0)
    --player2_pos, change_action = true;
  // Update ball position
  float new_ball_pos_x = ball_pos_x + ball_mov_x,
        new_ball_pos_y = ball_pos_y + ball_mov_y,
  		cy;
  if(line_x_vline(ball_pos_x, ball_pos_y, new_ball_pos_x, new_ball_pos_y, PLAYER1_X + 1, player1_pos, PLAYER_SIZE, &cy)){	// Hit player 1
    tone(BUZZER, SOUND_FREQUENCY, SOUND_DURATION);
    player_reflect((cy - player1_pos) / PLAYER_SIZE * 3);
    new_ball_pos_x = ball_pos_x + ball_mov_x;
    new_ball_pos_y = ball_pos_y + ball_mov_y;
  }else if(line_x_vline(ball_pos_x, ball_pos_y, new_ball_pos_x, new_ball_pos_y, PLAYER2_X, player2_pos, PLAYER_SIZE, &cy)){	// Hit player 2
    tone(BUZZER, SOUND_FREQUENCY, SOUND_DURATION);
    player_reflect((cy - player2_pos) / PLAYER_SIZE * 3);
    new_ball_pos_x = ball_pos_x + ball_mov_x;
    new_ball_pos_y = ball_pos_y + ball_mov_y;
  }else if(new_ball_pos_x < PLAYER1_X + 0.5f){	// Behind player 1
    g_clear();
    g_draw_text(0, 0, "Player 2 won!");
    while(true)
      delay(1000);
  }else if(new_ball_pos_x > PLAYER2_X + 0.5f){	// Behind player 2
    g_clear();
    g_draw_text(0, 0, "Player 1 won!");
    while(true)
      delay(1000);
  }
  if(new_ball_pos_y < 0.5f || new_ball_pos_y > LCD_PIXEL_HEIGHT-0.5f){	// Hit horizontal wall
    tone(BUZZER, SOUND_FREQUENCY, SOUND_DURATION);
    ball_mov_y = -ball_mov_y;
    new_ball_pos_y = ball_pos_y + ball_mov_y;
  }
  if(floor(ball_pos_x) != floor(new_ball_pos_x) || floor(ball_pos_y) != floor(new_ball_pos_y))	// Ball changed displayed position
    change_action = true;
  ball_pos_x = new_ball_pos_x;
  ball_pos_y = new_ball_pos_y;
  // Draw entities if needed
  if(change_action){
    g_clear();
    g_add_vline(PLAYER1_X, player1_pos, PLAYER_SIZE);
    g_add_vline(PLAYER2_X, player2_pos, PLAYER_SIZE);
    g_add_dot(ball_pos_x, ball_pos_y);
    g_draw();
  }
  change_action = false;
  // Pause between updates (setting - calculations duration)
  delay(map(analogRead(POTENTIOMETER), 0, 1023, MAX_UPDATE_DELAY, MIN_UPDATE_DELAY) - (millis() - profile_start_time));
}

// Serial data ready callback (blocked by loop)
void serialEvent(void){
  const String input = Serial.readString();
  // Ball position cheat
  unsigned bposx, bposy;
  if(sscanf(input.c_str(), "BPOS %u %u", &bposx, &bposy) == 2) // Scanning floats don't work without special compiler options
    ball_pos_x = bposx, ball_pos_y = bposy;
  // Pause game
  else if(input.equals("PAUSE"))
    do
      delay(1000);
    while(!Serial.available() || Serial.readString().length() == 0);
  else
    Serial.println("Invalid input!");
}