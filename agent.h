#include <cstdint>
#include <tuple>
#include <vector>
#include <unistd.h>

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
        bool sense_surroundings();
        void check_knowledge();
        void print_cave();
        void print_knowledge();
        std::vector<int> path_to(int x, int y);
        bool move_to(int x, int y);
        int get_x();
        int get_y();

        //squares we have already visited
        std::vector<std::tuple<int, int> > visited;

        //squares we have see but not visited
        std::vector<std::tuple<int, int> > seen;
        bool is_dead();

    private:
        //finds path to a tile; gives next move
        int manhattan(int x1, int y1, int x2, int y2);
        int space_in(int x, int y, std::vector<std::tuple< int, int, int, int, int> >);
        int space_in(int x, int y, std::vector<std::tuple< int, int> >);
        bool is_safe(int x, int y);
        bool grab();
        int points;
        int curr_x;
        int curr_y;
        int facing;
        bool arrow;
        bool dead;
        bool supmuw_food;
        bool has_gold;

        int cave_w;
        int cave_h;
        int init_x;
        int init_y;

        //the board tells what is actually in the square
// unused | dead wumpus | dead supmuw | wall | wumpus | gold | pit | supmuw
        uint8_t** board;

        //the knowledge pool says what things we have detected in what squares
        //if known is set, then the bits tell us what is in that square
// known | dead wumpus | dead supmuw | wall | wumpus | gold | pit | supmuw
        uint8_t** knowledge;
        //this is what we know doesn't exist in that space
        //a 0 indicates that thing cannot be in that square
        // unused | unused | unused | unused | wumpus | unused | pit | supmuw
        uint8_t** not_knowledge;
};
