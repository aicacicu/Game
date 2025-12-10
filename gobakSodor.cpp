#include <ncurses/ncurses.h>
#include <fstream>
#include <string>
#include <windows.h>
#include <ctime>
#include <cstring>

using namespace std;

const int MAP_H = 20;
const int MAP_W = 40;
const int totalPenjaga = 20;
const int totalLevel = 5;
const char* fileSkor = "simpanSkor.txt"; 
const char P1 = 'O';
const char P2 = '0';
const char GUARD = 'X';
const char FINISH = '=';
const int PLAY_W = 50;
const int PLAY_H = 20;
const int TICK_MS = 100;
int levelMaksTersedia = 1;
int skorMain[totalLevel+1] = {0};

struct Guard {
    int x, y;     
    int dir;      
    int speed;    
    int tick;     
    bool active;
};
struct Player {
    char name[32];
    int x, y;
    int status;   
};

string formatWaktu(int seconds) {
    int mm = seconds / 60;
    int ss = seconds % 60;
    char buf[16];
    sprintf(buf,"%02d:%02d", mm, ss);
    return string(buf);
}

void init_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(0);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
}
