#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "agent.h"

uint8_t get_tile_val(char tile);

int main(int argc, char** argv) {
    std::fstream file;
    file.open("wumpus.txt");
    int w;
    int h;
    char garbage[256];
    file >> w;
    file >> h;
    file.getline(garbage, 256);
    char** cave = new char*[h];
    for (int i = 0; i < h; i++) {
        char* row = new char[w];
        file.getline(row, w+1);
        cave[i] = row;
    }

    for (int i = 0; i < h; i++) {
        printf("%s\n", cave[i]);
    }
    printf("w: %d, h: %d\n", w, h);

    uint8_t** cave_vals = new uint8_t*[h];
    for (int i = 0; i < h; i++) {
        cave_vals[i] = new uint8_t[w];
        for (int j = 0; j < w; j++) {
            cave_vals[i][j] = get_tile_val(cave[i][j]);
        }
    }
    
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            printf("%2x", cave_vals[i][j]);
        }
        printf("\n");
    }


    for (int i = 0; i < h; i++) {
        delete[] cave[i];
    }
    delete[] cave;
}

// unused | unused | unused | wall | wumpus | gold | pit | supmuw
uint8_t get_tile_val(char tile) {
    switch (tile) {
        //inner wall
        case '#':
            return 0x10;

        //wumpus
        case 'W':
        case 'w':
            return 0x08;

        //pit
        case 'P':
        case 'p':
            return 0x02;

        //gold
        case 'G':
        case 'g':
            return 0x04;

        //supmuw
        case 'S':
        case 's':
            return 0x01;

        default:
            return 0x00;
    }
}
