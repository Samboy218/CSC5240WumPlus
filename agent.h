#include <cstdint>

class Agent {
    public:

        //we're going to give the agent the cave, 
        //but I swear he won't use it to cheat
        Agent(uint8_t** cave, int w, int h, int x, int y);
        ~Agent();
        //0 > up
        //1 > right
        //2 > down
        //3 > left
        //0 if a wall is there, -1 if we die, 1 if we move uneventfully
        int move(int dir);
        void turn(int dir);
        //0 if miss, -1 if no ammo, 1 if hit
        int shoot();

        //status is stored as several bits, if a bit is 1 we sensed that thing
        //we will only see dead if we are in that square
// unused | dead wumpus | dead supmuw | bump | smell | glitter | breeze | moo
        uint8_t detect();
        void sense_surroundings();
        void print_cave();
        void print_knowledge();

    private:
        int points;
        int curr_x;
        int curr_y;
        int facing;
        bool arrow;

        int cave_w;
        int cave_h;
        //the board tells what is actually in the square
// unused | dead wumpus | dead supmuw | wall | wumpus | gold | pit | supmuw
        uint8_t** board;

        //the knowledge pool says what things we have detected in what squares
        //if known is set, then the bits tell us what is in that square
// known | dead wumpus | dead supmuw | wall | wumpus | gold | pit | supmuw
        uint8_t** knowledge;
};
