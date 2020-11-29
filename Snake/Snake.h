#ifndef Snake_h
#define Snake_h

#include "Arduino.h"
#include <FastLED.h>

class Snake {
    public:
    Snake(int initial_r, int initial_c, CRGB c_body, CRGB c_body_fade, 
    CRGB c_tail, CRGB c_tail_fade, int num_links);
    
    // Called on the head of the snake, which calls move on itself
    int moveHead(int dir, int width, int height, CRGB* leds);

    // Checks if the head of this snake has intersected the other snake
    // If so, and this snake isn't faded and it intersected the other snake's tail,
    // this player wins. If the snake is faded, keep going. If the snake isn't faded 
    // and is not touching the tail, fade it.
    // returns 0 if no collision
    // returns 1 if a body collision or faded tail collision
    // returns 2 if an unfaded tail collision
    int checkCollision(int width, CRGB* leds, Snake* other_snake);

    // Checks if any links of this snake is intersecting the other snake
    bool checkIntersection(Snake* other_snake);
    void fadeColor(bool fade, int width, CRGB* leds);
    bool isFaded();
    void setColor(int width, CRGB* leds);
    void reset(int reset_r, int reset_c);

    private:
    // checks if any of this snake's links are at location (check_r, check_c)
    // returns 0 if no links are at the location
    // returns 1 if a body link is at the location
    // returns 2 if the tail link is at the location
    int isIntersecting(int check_r, int check_c);
    
    // moves the link to a new position and sets the color
    int move(int new_r, int new_c, int width, CRGB* leds);
    
    int r, c;
    int last_move;
    int back_move;
    CRGB color_cur, color_reg, color_fade;
    bool faded;
    Snake* next;
};
#endif