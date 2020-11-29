
#include "Snake.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

Snake::Snake(int initial_r, 
             int initial_c, 
             CRGB c_body, 
             CRGB c_body_fade, 
             CRGB c_tail, 
             CRGB c_tail_fade, 
             int num_links) {
    
    r = initial_r;
    c = initial_c;
    last_move = UP;
    back_move = DOWN;
    faded = false;
    if(num_links != 0) {
        color_cur = c_body;
        color_reg = c_body;
        color_fade = c_body_fade;

        next = new Snake(r-1, c, c_body, c_body_fade, c_tail, c_tail_fade, num_links-1);
    }
    else {
        color_cur = c_tail;
        color_reg = c_tail;
        color_fade = c_tail_fade;
        next = nullptr;
    }
}

void Snake::reset(int reset_r, int reset_c) {
    r = reset_r;
    c = reset_c;
    color_cur = color_reg;
    faded = false;
    last_move = UP;
    back_move = DOWN;

    if(next != nullptr) {
        next->reset(reset_r+1, reset_c);
    }
}

int Snake::moveHead(int dir, int width, int height, CRGB* leds) {
    int _dir = dir;
    if(_dir == back_move) {
        _dir = last_move;
    }
    int next_r = r, next_c = c;
    if(_dir == UP) {
        if(r == 0) { // loop from the top
            next_r = height-1;
        } else {
            next_r--;
        }
        last_move=UP;
        back_move=DOWN;
    }
    else if(_dir == RIGHT) { // Right
        if(c == width-1) { // loop from the right
            next_c = 0;
        } else {
            next_c++;
        }
        last_move=RIGHT;
        back_move=LEFT;
    }
    else if(_dir == DOWN) { // Down
        if(r == height-1) { // loop from the bottom
            next_r = 0;
        } else {
            next_r++;
        }
        last_move=DOWN;
        back_move=UP;
    }
    else if(_dir == LEFT) { // Left
        if(c == 0) {
            next_c = width-1;
        } else {
            next_c--;
        }
        last_move=LEFT;
        back_move=RIGHT;
    }

    move(next_r, next_c, width, leds);
}

int Snake::checkCollision(int width, CRGB* leds, Snake* other_snake) {
    int collision = other_snake->isIntersecting(r, c);
    if(collision == 0) {
        return 0;
    }
    if(collision==2 && !faded) {
        return 2;
    }
    if(collision==2 && faded) {
        return 1;
    }
    if(collision==1) {
        if(!faded) {
            fadeColor(true, width, leds);
        }
        return 1;
    }
    return -1;
}

bool Snake::checkIntersection(Snake* other_snake) {
    int intersect = other_snake->isIntersecting(r, c);
    if(intersect != 0) {
        return true;
    }
    else {
        if(next == nullptr) {
            return false;
        }
        return next->checkIntersection(other_snake);
    }
}

void Snake::fadeColor(bool fade, int width, CRGB* leds) {
    if(fade == faded) {
        return;
    }
    color_cur = (fade) ? color_fade : color_reg;
    faded = fade;
    leds[r * width + c] = color_cur;
    if(next != nullptr) {
        next->fadeColor(fade, width, leds);
    }
}

bool Snake::isFaded() {
    return faded;
}

void Snake::setColor(int width, CRGB* leds) {
    leds[r * width + c] = color_cur;
    if(next != nullptr) {
        next->setColor(width, leds);
    }
}



int Snake::isIntersecting(int check_r, int check_c) {
    if(check_r == r && check_c == c) {
        return (next != nullptr) ? 1 : 2;
    }
    else {
        return (next != nullptr) ? next->isIntersecting(check_r, check_c) : 0;
    }
}

int Snake::move(int new_r, int new_c, int width, CRGB* leds) {
    if(next == nullptr) {
        leds[r * width + c] = CRGB::Black;
        r = new_r;
        c = new_c;
        leds[r * width + c] = color_cur;
        return 0;
    }
    int last_r = r, last_c = c;
    r = new_r;
    c = new_c;
    leds[r * width + c] = color_cur;
    return next->move(last_r, last_c, width, leds);
}