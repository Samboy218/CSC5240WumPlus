#include "agent.h"

Agent::Agent(uint8_t** cave, int w, int h) {
}

Agent::~Agent() {
}

//0 > up
//1 > right
//2 > down
//3 > left
//0 if a wall is there, -1 if we die, 1 if we move uneventfully
int Agent::move(int dir) {
    points -= 1;
    uint8_t x_offset = 0;
    uint8_t y_offset = 0;
    switch (dir) {
        case 0:
            y_offset = 1;
            break;
        case 1:
            x_offset = 1;
            break;
        case 2:
            y_offset = -1;
            break;
        case 3:
            x_offset = -1;
            break;
        default:
            break;
    }
    uint8_t target = board[curr_y + y_offset][curr_x + x_offset];
    if (target & 0x10) {
        //we hit a wall
        return 0;
    }
    else { 
        curr_y += y_offset;
        curr_x += x_offset;
        //check for living wumpus
        if (target & 0x48 == 0x08) {
        //we got eaten by a wumpus
            points -= 1000;
            return -1;
        }
        
        //check for living supmuw
        if (target & 0x21 == 0x01) {
            //there is a supmuw, check if can smell a wumpus
            if (detect() & 0x08) {
                //he smells a wumpus, we are dead
                points -= 1000;
                return -1;
            }
            //if the supmuw cannot smell a wumpus, then we are good
            else {
                //check if we got a pit wumpus
                if (target & 0x02) {
                    return 1;
                }
                else {
                    points += 100;
                    return 1;
                }
            }
        }
        if (target & 0x02) {
            //pit and no supmuw (or dead wumpus)
            points -= 1000;
            return -1;
        }

        return 1;
    }
}

void Agent::turn(int dir) {
    points -= 1;
    facing = dir;
}

//0 if miss, -1 if no ammo, 1 if hit wumpus, 2 if hit supmuw
int Agent::shoot() {
    points -= 10;
    if (!arrow) {
        return -1;
    }
    arrow = false;
    int x_offset = 0;
    int y_offset = 0;
    switch (facing) {
        case 0:
            y_offset = 1;
            break;
        case 1:
            x_offset = 1;
            break;
        case 2:
            y_offset = -1;
            break;
        case 3:
            x_offset = -1;
            break;
        default:
            break;
    }
    int curr_x_offset = x_offset;
    int curr_y_offset = y_offset;
    uint8_t target;
    //yes this infinite while loop is on purpose
    while (true) {
        target = board[curr_y + curr_y_offset][curr_x + curr_x_offset];
        //mask out gold
        switch (target & 0x7B)  {
            //living wumpus hit
            case 0x08:
                board[curr_y + curr_y_offset][curr_x + curr_x_offset] |= 0x40;
                return 1;
            //living supmuw hit
            case 0x01:
                board[curr_y + curr_y_offset][curr_x + curr_x_offset] |= 0x20;
                return 2;
            //both living hit (kill wumpus)
            case 0x09:
                board[curr_y + curr_y_offset][curr_x + curr_x_offset] |= 0x40;
                return 1;
            //wall hit
            case 0x02:
                return 0;

            //a pit will always go here, meaning the supmuw is saved by a pit
            //if there is anything dead, the arrow will also pass over
                //we don't have to worry about any case where one is dead
            default:
                //arrow continues traveling
                curr_x_offset += x_offset;
                curr_y_offset += y_offset;
        }
    }
    //we shouldn't get here
}

//status is stored as several bits, if a bit is 1 we sensed that thing
// unused | unused | unused | breeze | smell | glitter | bump | moo
uint8_t Agent::detect() {
    uint8_t sense = 0x00;
    //get the 'sense' bits from neighbors
    sense |= board[curr_y-1][curr_x];
    sense |= board[curr_y+1][curr_x];
    sense |= board[curr_y][curr_x-1];
    sense |= board[curr_y][curr_x+1];
    //we can only see gold or dead if we are in its square, so mask those out
    sense &= 0x1B;
    sense |= board[curr_y][curr_x];
    //remove unused bits from being set
    sense &= 0x7F;
    return sense;
}

