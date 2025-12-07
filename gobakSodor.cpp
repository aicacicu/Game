#include <ncurses/ncurses.h>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

const int MAP_H = 20;
const int MAP_W = 40;
const int totalLevel = 5;
const int totalPenjaga = 10;
const char* fileSkor = "score.txt"; 
int levelMaksTersedia = 1;
int p1x, p1y, p2x, p2y;

const char P1 = 'O';
const char P2  = '0';
const char GUARD = 'G';
const char FINISH = '=';

PLAY_W = 50;
PLAY_H = 25;
PLAY_X = 20;
PLAY_Y = 3;

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

void frame(WINDOW *playwin);
void posisi(int px, int py, int &sx, int &sy);
void penjaga(int level, Guard guards[], int &numGuards);
bool playLevel(int level, Player &p1, Player &p2, int &elapsed_seconds);
string formatWaktu(int seconds);


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

void put_centered(int row, const char *s) {
    int cols = COLS;
    int len = strlen(s);
    int x = (cols - len) / 2;
    mvprintw(row, x, "%s", s);
    refresh();
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
        mvprintw(16, (COLS - 80) / 2, "|____.| \\___/ |_____||__|__ ||__|\\_|     \\____| \\___/ |___/   \\___/ |__|\\_| ");

        attroff(COLOR_PAIR(7) | COLOR_PAIR(3) | COLOR_PAIR(1) | COLOR_PAIR(5) | COLOR_PAIR(4)); 
        refresh();
        Sleep(1500);
    }  
    int posisiY = 18; 
    int posisiX = (COLS - 15) / 2; 
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
            Sleep(200); 
        }
    }
    refresh();
}

void skorTertinggi() {
    ifstream file(fileSkor);
    if (file) {
        string baris;
        mvprintw(1, 1, "Waktu Tercepat Kamu Bermain"); 
        int y = 2;
        while (getline(file, baris)) {
            mvprintw(y++, 0, "%s", baris.c_str());
        }
        file.close();
        getch();
    }
}

void simpanSkorTertinggi(int level, double waktu) {
    fstream file(fileSkor, ios::in | ios::out | ios::app);
    if (file) {
        file << "Level " << level << ": " << waktu << " second\n";
        file.close();
    }
}

void menu() {
    int pilihan = 0;
    const char* opsi[] = { "Mulai Bermain", "Skor Tertinggi", "Keluar" };
    int jumlahOpsi = sizeof(opsi) / sizeof(opsi[0]);

    while (true) {
        clear();

        attron(COLOR_PAIR(4));
        mvprintw(LINES / 2 - 4, (COLS - 22) / 2, "GOBAK SODOR");
        attroff(COLOR_PAIR(4));

        for (int i = 0; i < jumlahOpsi; i++) {
            if (i == pilihan) {
                attron(A_REVERSE | COLOR_PAIR(7));
            } else {
                attron(COLOR_PAIR(6));
            }

            mvprintw(LINES / 2 + i, (COLS - strlen(opsi[i])) / 2, opsi[i]);

            attroff(A_REVERSE | COLOR_PAIR(7));
            attroff(COLOR_PAIR(6));
        }

        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (pilihan > 0) pilihan--;
                break;

            case KEY_DOWN:
                if (pilihan < jumlahOpsi - 1) sorot++;
                break;

            case '\n':   
                if (pilihan == 0) {
                    return; 
                } else if (pilihan == 1) {
                    clear();
                    skorTertinggi();
                } else if (pilihan == 2) {
                    endwin();
                    exit(0);
                }
                break;
        }
    }
}

void informasi() {
    clear();

    mvprintw(1, 2, "Gobak Sodor Digital ini adalah permainan tradisional Indonesia");
    mvprintw(2, 2, "yang dimainkan dengan cara melewati garis penjaga tanpa tertangkap.");
    mvprintw(3, 2, "Dua pemain harus bekerja sama untuk mencapai garis finish sambil");
    mvprintw(4, 2, "menghindari penjaga yang bergerak di dalam arena.");
    mvprintw(6, 2, "Pemain menang jika kedua pemain berhasil mencapai garis finish.");
    mvprintw(7, 2, "Jika salah satu pemain tertangkap, permainan berakhir dan dianggap kalah.");
    mvprintw(9, 2, "Gunakan kontrol gerak untuk menghindari penjaga dan terus bergerak");
    mvprintw(10, 2, "maju sampai mencapai akhir lapangan.");
    mvprintw(13, 2, "Kontrol Pemain:");
    mvprintw(15, 4, "Player 1 : Panah atas (Maju), panah bawah (Mundur),");
    mvprintw(16, 4, "           panah kiri (Kiri), panah kanan (Kanan)");
    mvprintw(18, 4, "Player 2 : W (Maju), A (Kiri), S (Mundur), D (Kanan)");
    mvprintw(21, 2, "Tekan ENTER untuk melanjutkan...");

    refresh();
    timeout(-1);
    while (getch() != '\n') { 
	}
    timeout(0);
}

void namaPlayer(Player &p1, Player &p2) {
    echo();         
    curs_set(1);    
    timeout(-1);   
    clear();

    char name1[32];
    char name2[32];

    mvprintw(2, 2, "Masukkan Nama Pemain:");
    mvprintw(4, 2, "Player 1 : ");
    mvprintw(6, 2, "Player 2 : ");

    mvgetnstr(4, 22, name1, 30);
    mvgetnstr(6, 22, name2, 30);

    if (strlen(name1) == 0) strcpy(p1.name, "Player1");
    else strcpy(p1.name, name1);

    if (strlen(name2) == 0) strcpy(p2.name, "Player2");
    else strcpy(p2.name, name2);

    noecho();
    curs_set(0);
    timeout(0); 
}

int menuLevel() {
    const int totalLevel = 5; 
    int pilihan = 0;             

    while (true) {
        clear();
        
		attron(COLOR_PAIR(4));
        mvprintw(7, (COLS - 13) / 2, "PILIHAN LEVEL");
        attroff(COLOR_PAIR(4));
        
        int startY = LINES / 2 - totalLevel;

        for (int i = 0; i < totalLevel; i++) {
            if (i == pilihan) {
                attron(COLOR_PAIR(7) | A_REVERSE);
            } else {
                attron(COLOR_PAIR(6));
            }
            mvprintw(startY + i, (COLS - 10) / 2, "Level %d", i + 1);
            attroff(COLOR_PAIR(6));
            attroff(COLOR_PAIR(7) | A_REVERSE); 
        }

        int posBack = startY + totalLevel + 1;
        if (pilihan == totalLevel) {
                attron(COLOR_PAIR(7) | A_REVERSE);
            } else {
                attron(COLOR_PAIR(6));
        }
        mvprintw(posBack, (COLS - 13) / 2, "Keluar Game");
        attroff(COLOR_PAIR(6));
        attroff(COLOR_PAIR(7) | A_REVERSE); 
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (pilihan > 0) pilihan--;
                break;

            case KEY_DOWN:
                if (pilihan < totalLevel) pilihan++; 
                break;

            case '\n':  
                if (pilihan == totalLevel)
                    return -1;        
                else
                    return pilihan + 1; 
        }
    }
}

void border(WINDOW *mainwin, Player &p1, Player &p2, int waktu, int level) {
    clear();  

    attron(A_BOLD);
    mvprintw(0, 2, "Gobak Sodor");
    attroff(A_BOLD);

    mvprintw(1, 2, "P1: %s %s", p1.name,
             (p1.status==0)?"Bermain":(p1.status==1?"Tertangkap":"Finish"));
    mvprintw(2, 2, "P2: %s %s", p2.name,
             (p2.status==0)?"Bermain":(p2.status==1?"Tertangkap":"Finish"));

    if (waktu >= 0) {
        mvprintw(1, COLS - 15, "Waktu: %s", format_time(waktu).c_str());
    }

    if (level > 0) {
        mvprintw(2, COLS - 18, "Level: %2d / %2d", level, NUM_LEVELS);
    }

    mvhline(start_y, start_x, '-', width);
    mvhline(LINES-1, start_x, '-', width);

    mvvline(start_y, start_x, '|', height);
    mvvline(start_y, COLS-1, '|', height);

    mvaddch(start_y, start_x,'+');
    mvaddch(start_y, COLS-1,'+');
    mvaddch(LINES-1, start_x,'+');
    mvaddch(LINES-1, COLS-1,'+');

    refresh();
}

void frame(WINDOW *playwin) {
    wclear(playwin);
    box(playwin, 0, 0);

    int lanes = 4; 
    int height = PLAY_H;
    for (int i = 1; i <= lanes; ++i) {
        int yy = i * (height / (lanes + 1));
        for (int x = 1; x < PLAY_W-1; ++x) {
            mvwaddch(playwin, yy, x, '-');
        }
    }
    for (int c = 1; c <= 2; ++c) {
        int xx = c * (PLAY_W / 3);
        for (int y = 1; y < PLAY_H-1; ++y) {
            mvwaddch(playwin, y, xx, '|');
        }
    }
    for (int x = 1; x < PLAY_W-1; ++x) mvwaddch(playwin, 1, x, GOAL_CH);
    wrefresh(playwin);
}








