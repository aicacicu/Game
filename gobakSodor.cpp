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

void opening() {
	for (int i = 0; i< 5; i++) {
        switch (i) { 
            case 0: attron(COLOR_PAIR(7));
			break;  
            case 1: attron(COLOR_PAIR(3)); 
			break; 
            case 2: attron(COLOR_PAIR(1)); 
			break;  
            case 3: attron(COLOR_PAIR(4)); 
			break; 
            case 4: attron(COLOR_PAIR(6));
			break; 
        }
        mvprintw(10, (COLS - 80) / 2, "  ____   ___   ____    ____   __  _       _____  ___   ___     ___   ____   ");
        mvprintw(11, (COLS - 80) / 2, " /    | /   \\ |    \\  /    \\ |  |/ |     / ___/ /   \\ |   \\   /   \\ |    \\  ");
        mvprintw(12, (COLS - 80) / 2, "|   __||     ||  O  )|  O   ||  ' /     /   \\_ |     ||    \\ |     ||  D  ) ");
        mvprintw(13, (COLS - 80) / 2, "|  | _ |  O  ||    | |      ||    \\     \\__   ||  O  ||  D  ||  O  ||    /  ");
        mvprintw(14, (COLS - 80) / 2, "|  |  ||     ||  O  )|  _   ||     |    /  \\  ||     ||     ||     ||    \\  ");
        mvprintw(15, (COLS - 80) / 2, "|     ||     ||     ||  |   ||  .  |    \\     ||     ||    / |     ||  .  \\ ");
        mvprintw(16, (COLS - 80) / 2, "|____.| \\___/ |_____||__|___||__|\\_|     \\____| \\___/ |___/   \\___/ |__|\\_| ");
        attroff(COLOR_PAIR(7) | COLOR_PAIR(3) | COLOR_PAIR(1) | COLOR_PAIR(5) | COLOR_PAIR(4)); 
        refresh();
        Sleep(1000);
    }  
    int posisiY = 18; 
    int posisiX = (COLS - 21) / 2; 
    for (int i = 0; i <= 3; i++) {
    	attron(COLOR_PAIR(6));
        mvprintw(posisiY, posisiX, "-------------------");
        mvprintw(posisiY + 1, posisiX, "[                 ]");
        mvprintw(posisiY + 2, posisiX, "-------------------");
        attroff(COLOR_PAIR(6));

        for (int j = 0; j <= 15; j++) {
        	attron(COLOR_PAIR(4));
            mvprintw(posisiY + 1, posisiX + j + 1, ">");
            attroff(COLOR_PAIR(4));
            refresh();
            Sleep(100); 
        }
    }
    refresh();
}

void namaPlayer(Player &p1, Player &p2) {
    echo();
    curs_set(1);
    timeout(-1);
    clear();

    char name1[31], name2[31];
    mvprintw(10, (COLS - 19) / 2, "Masukkan Nama Pemain:");
    mvprintw(13, 50, "Player 1 : ");
    mvprintw(14, 50, "Player 2 : ");

    mvgetnstr(13, 62, name1, 30);
    mvgetnstr(14, 62, name2, 30);

    strcpy(p1.name, strlen(name1) == 0 ? "Player1" : name1);
    strcpy(p2.name, strlen(name2) == 0 ? "Player2" : name2);

    noecho();
    curs_set(0);
    timeout(0);
}
