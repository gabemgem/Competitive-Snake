#include <FastLED.h>

#include <Snake.h>

// #define DEBUG

#define LED_PIN 13
#define P1_LED_PIN 0
#define P2_LED_PIN 1

#define NUM_LEDS 64
#define GRID_WIDTH 8
#define GRID_HEIGHT 7

#define BRIGHTNESS 150

CRGB leds[NUM_LEDS];
CRGB p1_led[1];
CRGB p2_led[1];

#define NUM_SNAKE_LINKS 4

#define PLAYER_ONE_UP 2
#define PLAYER_ONE_DOWN 3
#define PLAYER_ONE_LEFT 4
#define PLAYER_ONE_RIGHT 5
#define PLAYER_ONE_ENTER 6

#define PLAYER_TWO_UP 7
#define PLAYER_TWO_DOWN 8
#define PLAYER_TWO_LEFT 9
#define PLAYER_TWO_RIGHT 10
#define PLAYER_TWO_ENTER 11

int player_one_up_state = 1;
int player_one_down_state = 1;
int player_one_left_state = 1;
int player_one_right_state = 1;
int player_one_enter_state = 1;
int player_one_up_state_cur = 1;
int player_one_down_state_cur = 1;
int player_one_left_state_cur = 1;
int player_one_right_state_cur = 1;
int player_one_enter_state_cur = 1;
unsigned long player_one_enter_pressed_time = 0;

int player_two_up_state = 1;
int player_two_down_state = 1;
int player_two_left_state = 1;
int player_two_right_state = 1;
int player_two_enter_state = 1;
int player_two_up_state_cur = 1;
int player_two_down_state_cur = 1;
int player_two_left_state_cur = 1;
int player_two_right_state_cur = 1;
int player_two_enter_state_cur = 1;
unsigned long player_two_enter_pressed_time = 0;

int player_one_moves_max = 4;
int player_two_moves_max = 4;
int player_one_last_move = 0;
int player_two_last_move = 0;
int player_one_moves[4];
int player_two_moves[4];

CRGB p1_color_main = CRGB::Crimson;
CRGB p1_color_main_fade = CRGB::LightCoral;
CRGB p1_color_tail = CRGB::DarkRed;
CRGB p1_color_tail_fade = CRGB::FireBrick;

CRGB p2_color_main = CRGB::DodgerBlue;
CRGB p2_color_main_fade = CRGB::DarkTurquoise;
CRGB p2_color_tail = CRGB::DarkBlue;
CRGB p2_color_tail_fade = CRGB::DeepSkyBlue;

bool p1_won = false;
bool p2_won = false;

Snake p1(1, 1, p1_color_main, p1_color_main_fade, p1_color_tail, p1_color_tail_fade, NUM_SNAKE_LINKS-1);
Snake p2(1, GRID_WIDTH-2, p2_color_main, p2_color_main_fade, p2_color_tail, p2_color_tail_fade, NUM_SNAKE_LINKS-1);

void sprint(String msg) {
  #ifdef DEBUG
  Serial.print(msg);
  #endif
}

void set_player_leds(CRGB p1_color, CRGB p2_color) {
  #ifndef DEBUG
  p1_led[0] = p1_color;
  p2_led[0] = p2_color;
  FastLED.show();
  #endif
}

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #else

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds<WS2812, P1_LED_PIN, GRB>(p1_led, 1);
  FastLED.addLeds<WS2812, P2_LED_PIN, GRB>(p2_led, 1);
  #endif
  sprint("Started up!\n");
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  pinMode(PLAYER_ONE_UP, INPUT_PULLUP);
  pinMode(PLAYER_ONE_DOWN, INPUT_PULLUP);
  pinMode(PLAYER_ONE_LEFT, INPUT_PULLUP);
  pinMode(PLAYER_ONE_RIGHT, INPUT_PULLUP);
  pinMode(PLAYER_ONE_ENTER, INPUT_PULLUP);

  pinMode(PLAYER_TWO_UP, INPUT_PULLUP);
  pinMode(PLAYER_TWO_DOWN, INPUT_PULLUP);
  pinMode(PLAYER_TWO_LEFT, INPUT_PULLUP);
  pinMode(PLAYER_TWO_RIGHT, INPUT_PULLUP);
  pinMode(PLAYER_TWO_ENTER, INPUT_PULLUP);

  fill_solid(leds, NUM_LEDS, CRGB::Grey);
  FastLED.show();
  
  delay(2000);

  resetGame();
}

void loop() {
  if(p1_won || p2_won) {
    if(p1_won) {
      fill_solid(leds, NUM_LEDS, p1_color_main);
      FastLED.show();
      delay(1500);
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(500);
    }
    if(p2_won) {
      fill_solid(leds, NUM_LEDS, p2_color_main);
      FastLED.show();
      delay(1500);
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(500);
    }

    player_one_enter_state_cur = digitalRead(PLAYER_ONE_ENTER);
    int player_one_pressed_enter = pressedEnter(player_one_enter_state_cur, player_one_enter_state, player_one_enter_pressed_time);
    player_two_enter_state_cur = digitalRead(PLAYER_TWO_ENTER);
    int player_two_pressed_enter = pressedEnter(player_two_enter_state_cur, player_two_enter_state, player_two_enter_pressed_time);

    if(player_one_pressed_enter > 0 || player_two_pressed_enter > 0) {
      resetGame();
    }
  }
  else {
    int player_one_moves_count = 0;
    int player_two_moves_count = 0;
    //monitor buttons
    bool player_one_ready = false;
    bool player_two_ready = false;

    // Up=0; Down=2; Left=3; Right=1;
    while(!player_one_ready || !player_two_ready) {
      
      player_one_enter_state_cur = digitalRead(PLAYER_ONE_ENTER);
      int player_one_pressed_enter = pressedEnter(player_one_enter_state_cur, player_one_enter_state, player_one_enter_pressed_time);
      if(player_one_pressed_enter == 1) {
        player_one_ready = true;
        sprint("p1-enter-reg\n");
      }
      else if(player_one_pressed_enter == 2) {
        player_one_moves_count = 0;
        player_one_ready = false;
        sprint("p1-enter-reset\n");
      }

      player_two_enter_state_cur = digitalRead(PLAYER_TWO_ENTER);
      int player_two_pressed_enter = pressedEnter(player_two_enter_state_cur, player_two_enter_state, player_two_enter_pressed_time);
      if(player_two_pressed_enter == 1) {
        player_two_ready = true;
          sprint("p2-enter-reg\n");
      }
      else if(player_two_pressed_enter == 2) {
        player_two_moves_count = 0;
        player_two_ready = false;
        sprint("p2-enter-reset\n");
      }
      
      if(!player_one_ready) {
        if(player_one_moves_count<player_one_moves_max) {
          player_one_up_state_cur = digitalRead(PLAYER_ONE_UP);
          player_one_down_state_cur = digitalRead(PLAYER_ONE_DOWN);
          player_one_left_state_cur = digitalRead(PLAYER_ONE_LEFT);
          player_one_right_state_cur = digitalRead(PLAYER_ONE_RIGHT);
          
  
          if(pressedButton(player_one_up_state_cur, player_one_up_state)) {
            player_one_moves[player_one_moves_count] = 0;
            player_one_moves_count++;
            sprint("p1-up\n");
          }
          if(pressedButton(player_one_down_state_cur, player_one_down_state)) {
            player_one_moves[player_one_moves_count] = 2;
            player_one_moves_count++;
            sprint("p1-down\n");
          }
          if(pressedButton(player_one_left_state_cur, player_one_left_state)) {
            player_one_moves[player_one_moves_count] = 3;
            player_one_moves_count++;
            sprint("p1-left\n");
          }
          if(pressedButton(player_one_right_state_cur, player_one_right_state)) {
            player_one_moves[player_one_moves_count] = 1;
            player_one_moves_count++;
            sprint("p1-right\n");
          }
        }
      }
      
      if(!player_two_ready) {
        if(player_two_moves_count<player_two_moves_max) {
          player_two_up_state_cur = digitalRead(PLAYER_TWO_UP);
          player_two_down_state_cur = digitalRead(PLAYER_TWO_DOWN);
          player_two_left_state_cur = digitalRead(PLAYER_TWO_LEFT);
          player_two_right_state_cur = digitalRead(PLAYER_TWO_RIGHT);
          
  
          if(pressedButton(player_two_up_state_cur, player_two_up_state)) {
            player_two_moves[player_two_moves_count] = 0;
            player_two_moves_count++;
            sprint("p2-up\n");
          }
          if(pressedButton(player_two_down_state_cur, player_two_down_state)) {
            player_two_moves[player_two_moves_count] = 2;
            player_two_moves_count++;
            sprint("p2-down\n");
          }
          if(pressedButton(player_two_left_state_cur, player_two_left_state)) {
            player_two_moves[player_two_moves_count] = 3;
            player_two_moves_count++;
            sprint("p2-left\n");
          }
          if(pressedButton(player_two_right_state_cur, player_two_right_state)) {
            player_two_moves[player_two_moves_count] = 1;
            player_two_moves_count++;
            sprint("p2-right\n");
          }
        }
      }

      CRGB p1_led_color = (player_one_ready || player_one_moves_count == player_one_moves_max) ? p1_color_main : CRGB::Black;
      CRGB p2_led_color = (player_two_ready || player_two_moves_count == player_two_moves_max) ? p2_color_main : CRGB::Black;
      set_player_leds(p1_led_color, p2_led_color);

      delay(50);
    }

    if(player_one_moves_count == 0) {
      player_one_moves[0] = player_one_last_move;
      player_one_moves_count++;
    }
    if(player_two_moves_count == 0) {
      player_two_moves[0] = player_two_last_move;
      player_two_moves_count++;
    }
    player_one_last_move = player_one_moves[player_one_moves_count-1];
    player_two_last_move = player_two_moves[player_two_moves_count-1];

    #ifdef DEBUG
    Serial.print("Player one moves: ");
    for(int i=0; i<player_one_moves_count; i++) {
      Serial.print(player_one_moves[i]);
      Serial.print(", ");
    }
    Serial.print("\nPlayer two moves: ");
    for(int i=0; i<player_two_moves_count; i++) {
      Serial.print(player_two_moves[i]);
      Serial.print(", ");
    }
    Serial.print("\n\n");
    #endif
    
    player_one_moves_max = 5 - player_one_moves_count;
    player_two_moves_max = 5 - player_two_moves_count;
    //move snakes
    moveSnakes(player_one_moves_count, player_two_moves_count);
  }
}


void resetGame() {
  sprint("Resetting...");
  p1_won = false;
  p2_won = false;

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  p1.reset(1, 1);
  p2.reset(1, GRID_WIDTH-2);

  p1.setColor(GRID_WIDTH, leds);
  p2.setColor(GRID_WIDTH, leds);
  FastLED.show();
  delay(30);
  sprint("Reset\n");
}

int pressedEnter(int& cur_state, int& last_state, unsigned long& pressed_time) {
  if(cur_state != last_state) {
    last_state = cur_state;
    if(cur_state == 0) {
      pressed_time = millis();
      return 0;
    }
    else {
      unsigned long cur_time = millis();
      if(cur_time - pressed_time >= 1000) {
        return 2;
      }
      return 1;
    }
  }
  return 0;
}

bool pressedButton(int& cur_state, int& last_state) {
  if(cur_state != last_state) {
    last_state = cur_state;
    if(cur_state == 0) {
      return true;
    }
  }
  return false;
}

void moveSnakes(int player_one_moves_count, int player_two_moves_count) {
  for(int i = 0; i<4; i++) {
      if(player_one_moves_count != 0) {
        player_one_moves_count--;
        p1.moveHead(player_one_moves[i], GRID_WIDTH, GRID_HEIGHT, leds);
        if(!p1.isFaded()) {
          int collision = p1.checkCollision(GRID_WIDTH, leds, &p2);
          if(collision == 2) {
            p1_won = true;
            sprint("p1-won\n");
          }
          else if(collision == 1) {
            p1.fadeColor(true, GRID_WIDTH, leds);
            sprint("p1-collided\n");
          }
        }
        else {
          bool intersecting = p1.checkIntersection(&p2);
          if(!intersecting) {
            p1.fadeColor(false, GRID_WIDTH, leds);
            sprint("p1-no-longer-collided\n");
          }
        }
      }
      if(player_two_moves_count != 0) {
        player_two_moves_count--;
        p2.moveHead(player_two_moves[i], GRID_WIDTH, GRID_HEIGHT, leds);
        if(!p2.isFaded()) {
          int collision = p2.checkCollision(GRID_WIDTH, leds, &p1);
          if(collision == 2) {
            p2_won = true;
            sprint("p2-won\n");
          }
          else if(collision == 1) {
            p2.fadeColor(true, GRID_WIDTH, leds);
            sprint("p2-collided\n");
          }
        }
        else {
          bool intersecting = p2.checkIntersection(&p1);
          if(!intersecting) {
            p2.fadeColor(false, GRID_WIDTH, leds);
            sprint("p2-no-longer-collided\n");
          }
        }
      }
      FastLED.show();
      delay(750);
      if(p1_won || p2_won) {
        break;
      }
    }
}
