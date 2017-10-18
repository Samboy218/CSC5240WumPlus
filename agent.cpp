#include <iostream>
#include "agent.h"

Agent::Agent(uint8_t** cave, int w, int h, int x, int y) {
    points = 0;
    curr_x = x;
    curr_y = y;
    facing = 0;
    arrow = true;
    cave_w = w;
    cave_h = h;
    board = cave;
    knowledge = new uint8_t*[cave_h];
    for (int i = 0; i < h; i++) {
        knowledge[i] = new uint8_t[cave_w];
        for ( int j = 0; j < w; j++) {
            if (j == 0 || i == 0 || j == w-1 || i == h-1) {
                knowledge[i][j] = 0x90;
            }
            else
                knowledge[i][j] = 0x00;
        }
    }
}

Agent::~Agent() {
    for (int i = 0; i < cave_h; i++) {
        delete[] knowledge[i];
    }
    delete[] knowledge;
}

//0 > up
//1 > right
//2 > down
//3 > left
//0 if a wall is there, -1 if we die, 1 if we move uneventfully
int Agent::move(int dir) {
    points -= 1;
    int x_offset = 0;
    int y_offset = 0;
    switch (dir) {
        case 0:
            y_offset = -1;
            break;
        case 1:
            x_offset = 1;
            break;
        case 2:
            y_offset = 1;
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
    sense &= 0x0B;
    sense |= board[curr_y][curr_x];
    //remove unused bits from being set
    sense &= 0x7F;
    return sense;
}

void Agent::sense_surroundings() {
    uint8_t sense = detect();
    //if we see gold or a dead thing, then we know what is in our square
    //don't modify a square with the knowledge bit set

    //if we see gold
    if (sense & 0x04) {
        knowledge[curr_y][curr_x] = 0x84;
        //there may also be a dead supmuw here, so check that
        knowledge[curr_y][curr_x] |= (sense & 0x20);
    }
    //dead wumpus
    else if (sense & 0x40) {
        //if we see dead wumpus, then nothing else is in this square
        knowledge[curr_y][curr_x] = 0xC0;
    }
    //dead supmuw, but no gold here
    else if (sense & 0x20) {
        knowledge[curr_y][curr_x] = 0xA0;
    }
    //other cases of our current space
    else {
        if (sense == 0) {
            knowledge[curr_y][curr_x] = 0x80;
        }
        //mask out wumpus and pit, if we don't die they aren't here
        knowledge[curr_y][curr_x] |= sense & 0x01;
    }
    //remove those already covered cases from the sense
    sense &= 0x0B;
    //left square
    //right squre
    //up square
    //down square
    if (!(knowledge[curr_y+1][curr_x] & 0x80)) {
        if (sense == 0) {
            knowledge[curr_y+1][curr_x] = 0x80;
        }
        knowledge[curr_y+1][curr_x] |= sense;
    }
    if (!(knowledge[curr_y-1][curr_x] & 0x80)) {
        if (sense == 0) {
            knowledge[curr_y-1][curr_x] = 0x80;
        }
        knowledge[curr_y-1][curr_x] |= sense;
    }
    if (!(knowledge[curr_y][curr_x+1] & 0x80)) {
        if (sense == 0) {
            knowledge[curr_y][curr_x+1] = 0x80;
        }
        knowledge[curr_y][curr_x+1] |= sense;
    }
    if (!(knowledge[curr_y][curr_x-1] & 0x80)) {
        if (sense == 0) {
            knowledge[curr_y][curr_x-1] = 0x80;
        }
        knowledge[curr_y][curr_x-1] |= sense;
    }

}

void Agent::print_cave() {
    for (int i = 0; i < cave_h; i++) {
        for (int j = 0; j < cave_w; j++) {
            if (j == curr_x && i == curr_y) {
                printf("A");
            }
            else {
                switch(board[i][j]) {
                    //pit
                    case 0x02:
                        //printf("O");
                        printf("\x1b[44;30mO\x1b[0m");
                        break;
                    //gold
                    case 0x04:
                        //printf("*");
                        printf("\x1b[33m*\x1b[0m");
                        break;
                    //wumpus
                    case 0x08:
                        //printf("W");
                        printf("\x1b[31mW\x1b[0m");
                        break;
                    //supmuw
                    case 0x01:
                        //printf("S");
                        printf("\x1b[32mS\x1b[0m");
                        break;
                    //wall
                    case 0x10:
                        //printf("#");
                        printf("\x1b[40m#\x1b[0m");
                        break;
                    //dead supmuw
                    case 0x21:
                        //printf("$");
                        printf("\x1b[32m$\x1b[0m");
                        break;
                    //dead wumpus
                    case 0x48:
                        //printf("X");
                        printf("\x1b[31mX\x1b[0m");
                        break;
                    //pit & supmuw
                    case 0x03:
                        //printf("@");
                        printf("\x1b[44;32m@\x1b[0m");
                        break;
                    //gold & supmuw
                    case 0x05:
                        //printf("&");
                        printf("\x1b[43;32m&\x1b[0m");
                        break;
                    //wumpus & supmuw
                    case 0x09:
                        //printf("%");
                        printf("\x1b[31m%\x1b[0m");
                        break;
                    default:
                        printf(" ");
                }
            }
        }
        printf("\n");
    }
}

void Agent::print_knowledge() {
    for (int i = 0; i < cave_h; i++) {
        for (int j = 0; j < cave_w; j++) {
            char space[10] = "         ";
            if (j == curr_x && i == curr_y) {
                space[0] = 'A';
            }
            //we don't know what is in the space
            if (!(knowledge[i][j] & 0x80)) {
                space[1] = '?';
            }
            //moo
            if (knowledge[i][j] & 0x01) {
                space[2] = 'S';
            }
            //pit (breeze)
            if (knowledge[i][j] & 0x02) {
                space[3] = '~';
            }
            //gold (glitter)
            if (knowledge[i][j] & 0x04) {
                space[4] = '*';
            }
            //Wumpus (smell)
            if (knowledge[i][j] & 0x08) {
                space[5] = 'W';
            }
            //wall
            if (knowledge[i][j] & 0x10) {
                space[6] = '#';
            }
            //dead supmuw
            if (knowledge[i][j] & 0x20) {
                space[7] = '$';
            }
            //dead wumpus
            if (knowledge[i][j] & 0x40) {
                space[8] = 'X';
            }
            printf("| %s ", space);
        }
        printf("|\n");
    }

}
