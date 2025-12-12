#include <ncurses/ncurses.h>
#include <fstream>
#include <string>
#include <windows.h>
#include <ctime>
#include <cstring>

using namespace std;

const int MAP_H = 20;
const int MAP_W = 40;
const int totalPenjaga = 12;
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
    char buf[10];
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

void informasi() {
    clear();
    
    mvprintw(5, (COLS - 62) / 2, "Gobak Sodor Digital ini adalah permainan tradisional Indonesia");
    mvprintw(6, (COLS - 68) / 2, "yang dimainkan dengan cara melewati garis penjaga tanpa tertangkap.");
    mvprintw(7, (COLS - 67) / 2, "Dua pemain harus bekerja sama untuk mencapai garis finish sambil");
    mvprintw(8, (COLS - 49) / 2, "menghindari penjaga yang bergerak di dalam arena.");
    mvprintw(10, (COLS - 55) / 2, "Pemain menang jika kedua pemain berhasil mencapai garis finish.");
    mvprintw(11, (COLS - 73) / 2, "Jika salah satu pemain tertangkap, permainan berakhir dan dianggap kalah.");
    mvprintw(12, (COLS - 68) / 2, "Gunakan kontrol gerak untuk menghindari penjaga dan terus bergerak");
    mvprintw(13, (COLS - 35) / 2, "maju sampai mencapai akhir lapangan.");
    mvprintw(15, 23, "Kontrol Pemain:");
    mvprintw(17, 23, "Player 1 (O) : Panah atas (Maju), panah bawah (Mundur)");
    mvprintw(18, 23, "           panah kiri (Kiri), panah kanan (Kanan)");
    mvprintw(20, 23, "Player 2 (0) : W (Maju), A (Kiri), S (Mundur), D (Kanan)");
    mvprintw(24, (COLS - 32) / 2, "Tekan ENTER untuk melanjutkan...");
    refresh();
    timeout(-1);
    while (getch() != '\n') 
    timeout(0);
}

void skor() {
    ifstream file(fileSkor);
    if (file) {
        string baris;
        mvprintw(0, (COLS - 23) / 2, "Waktu Bermain Per Level"); 
        int y = 2;
        while (getline(file, baris)) {
            mvprintw(y++, (COLS - 18) / 2, "%s", baris.c_str());
        }
        file.close();
        getch();
    }
}

void simpanSkor(int level, double waktu) {
    fstream file(fileSkor, ios::in | ios::out | ios::app);
    if (file) {
        file << "Level " << level << ": " << waktu << " second\n";
        file.close();
    }
}

void menu() {
	timeout(-1);
    erase();
    refresh();
	
    int pilihan = 0;
    const char* opsi[] = { "Mulai Bermain", "Lihat Skor", "Keluar" };
    int jumlahOpsi = sizeof(opsi) / sizeof(opsi[0]);

    while (true) {
        erase();
        attron(COLOR_PAIR(4));
        mvprintw(LINES / 2 - 5, (COLS - 11) / 2, "GOBAK SODOR");
        attroff(COLOR_PAIR(4));

        for (int i = 0; i < jumlahOpsi; i++) {
            int y = (LINES - jumlahOpsi) / 2 + i;               
             int x = (COLS - strlen(opsi[i])) / 2;
        	
            if (i == pilihan) attron(A_REVERSE | COLOR_PAIR(7));
            else attron(COLOR_PAIR(6));

            mvprintw(y, x, opsi[i]);

            attroff(A_REVERSE | COLOR_PAIR(7));
            attroff(COLOR_PAIR(6));
        }
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (pilihan > 0) pilihan--; break;
            case KEY_DOWN: if (pilihan < jumlahOpsi - 1) pilihan++; break;
            case '\n':
                if (pilihan == 0) return;
                else if (pilihan == 1) { clear(); skor(); }
                else if (pilihan == 2) { endwin(); exit(0); }
                break;
        }
    }
}

int menuLevel() {
    timeout(-1);
    erase();
    refresh();

    const int totalOpsi = totalLevel + 1;

    struct koordinat { 
        int y, x; 
    };
    koordinat opsi[totalOpsi];

    int mulaiY = LINES / 2 - 1;
    int mulaiX = (COLS - 29) / 2;

    for (int i = 0; i < totalLevel; i++) {
        int baris = mulaiY + (i >= 3 ? 2 : 0);
        int kolom = (i < 3) ? mulaiX + (i % 3) * 12 : mulaiX + 6 + ((i - 3) % 2) * 12;
        opsi[i] = { baris, kolom };
    }

    opsi[totalLevel] = { mulaiY + 4, mulaiX + 9 };

    int pilihan = 0;

    while (true) {
        clear();

        attron(COLOR_PAIR(4));
        mvprintw(LINES / 2 - 5 / 2 - 2, (COLS - 11) / 2, "PILIH LEVEL");
        attroff(COLOR_PAIR(4));

        for (int i = 0; i < totalLevel; i++) {
            if (i == pilihan) attron(A_REVERSE | COLOR_PAIR(7));
            else attron(COLOR_PAIR(6));

            mvprintw(opsi[i].y, opsi[i].x, 
                     i < levelMaksTersedia ? "Level %d" : "Locked", i + 1);

            attroff(A_REVERSE | COLOR_PAIR(7));
            attroff(COLOR_PAIR(6));
        }

        if (pilihan == totalLevel) attron(A_REVERSE | COLOR_PAIR(7));
        else attron(COLOR_PAIR(6));
        mvprintw(opsi[totalLevel].y, opsi[totalLevel].x, "Keluar Game");
        attroff(A_REVERSE | COLOR_PAIR(7));
        attroff(COLOR_PAIR(6));

        refresh();

        int ch = getch();
        int best, minDist;

        switch (ch) {
            case KEY_UP:
                best = pilihan;
                minDist = LINES;
                for (int i = 0; i < totalOpsi; i++) {
                    if (opsi[i].y < opsi[pilihan].y) {
                        int dist = abs(opsi[i].y - opsi[pilihan].y) + abs(opsi[i].x - opsi[pilihan].x);
                        if (dist < minDist) { best = i; minDist = dist; }
                    }
                }
                pilihan = best;
                break;

            case KEY_DOWN:
                best = pilihan;
                minDist = LINES;
                for (int i = 0; i < totalOpsi; i++) {
                    if (opsi[i].y > opsi[pilihan].y) {
                        int dist = abs(opsi[i].y - opsi[pilihan].y) + abs(opsi[i].x - opsi[pilihan].x);
                        if (dist < minDist) { best = i; minDist = dist; }
                    }
                }
                pilihan = best;
                break;

            case KEY_LEFT:
                best = pilihan;
                minDist = COLS;
                for (int i = 0; i < totalOpsi; i++) {
                    if (opsi[i].y == opsi[pilihan].y && opsi[i].x < opsi[pilihan].x) {
                        int dist = opsi[pilihan].x - opsi[i].x;
                        if (dist < minDist) { best = i; minDist = dist; }
                    }
                }
                pilihan = best;
                break;

            case KEY_RIGHT:
                best = pilihan;
                minDist = COLS;
                for (int i = 0; i < totalOpsi; i++) {
                    if (opsi[i].y == opsi[pilihan].y && opsi[i].x > opsi[pilihan].x) {
                        int dist = opsi[i].x - opsi[pilihan].x;
                        if (dist < minDist) { best = i; minDist = dist; }
                    }
                }
                pilihan = best;
                break;

            case '\n':
                if (pilihan == totalLevel) return -1;
                if (pilihan < levelMaksTersedia) return pilihan + 1;
                break;
        }
    }
}

void lapangan(WINDOW *playwin) {
    wclear(playwin);
    
    wattron(playwin, COLOR_PAIR(2));
    box(playwin, 0, 0);

    int lanes = 4, height = PLAY_H;

    for (int i = 1; i <= lanes; ++i) {
        int yy = i * (height / (lanes + 1));
        for (int x = 1; x < PLAY_W - 1; ++x) 
		mvwaddch(playwin, yy, x, '-');
    }
    for (int c = 1; c <= 2; ++c) {
        int xx = c * (PLAY_W / 3);
        for (int y = 1; y < PLAY_H - 1; ++y) 
		mvwaddch(playwin, y, xx, '|');
    }
    for (int x = 1; x < PLAY_W - 1; ++x) 
	mvwaddch(playwin, 1, x, FINISH);
	wattroff(playwin, COLOR_PAIR(2));
    
    wrefresh(playwin);
}

void posisiPemain(int px, int py, int &sx, int &sy) {
    sx = 1 + px;
    sy = 1 + py;
}

void PenjagaPerLevel(int level, Guard daftarPenjaga[], int &jumlahPenjaga) {
    jumlahPenjaga = 0;
    
    int jumlahLane = 2 + (level / 2);
    if (jumlahLane > 5) jumlahLane = 5;
    int jarakLane = (PLAY_H - 3) / (jumlahLane + 1);
    if (jarakLane < 1) jarakLane = 1;
    int placed = 0;

    for (int lane = 0; lane < jumlahLane; lane++) {
        int py = 1 + (lane + 1) * jarakLane;
        int penjagaPerLane = 1 + (level / 2);
        for (int g = 0; g < penjagaPerLane && placed < totalPenjaga; g++) {
            daftarPenjaga[placed].y = py;
            
            int jarak = (PLAY_W - 4) / penjagaPerLane;
            daftarPenjaga[placed].x = 1 + (g * jarak) % (PLAY_W - 2);
            daftarPenjaga[placed].dir = ((lane + g) % 2 == 0) ? 1 : -1;
            int base = 5 - (level / 2);
            if (base < 1) base = 1;
            daftarPenjaga[placed].speed = base;
            daftarPenjaga[placed].tick = 0;
            daftarPenjaga[placed].active = true;
            placed++;
        }
    }
    jumlahPenjaga = placed;
}

void areaPermainan(WINDOW *playwin, Player &p1, Player &p2, Guard daftarPenjaga[], int jumlahPenjaga) {
    lapangan(playwin);
    
    for (int i = 0; i < jumlahPenjaga; i++) {
        if (!daftarPenjaga[i].active) continue;
        
        int sx, sy;
        posisiPemain(daftarPenjaga[i].x, daftarPenjaga[i].y, sx, sy);
        wattron(playwin, COLOR_PAIR(1));
        mvwaddch(playwin, sy, sx, GUARD);
        wattroff(playwin, COLOR_PAIR(1));
    }
    
    int sx1, sy1, sx2, sy2;
    posisiPemain(p1.x, p1.y, sx1, sy1);
    posisiPemain(p2.x, p2.y, sx2, sy2);
    
    wattron(playwin, COLOR_PAIR(3));
    mvwaddch(playwin, sy1, sx1, P1);
    mvwaddch(playwin, sy2, sx2, P2);
    wattroff(playwin, COLOR_PAIR(3));
    
    wrefresh(playwin);
}

bool bermain(int level, Player &p1, Player &p2, int &waktuBermain) {
 	Guard daftarPenjaga[totalPenjaga];
    int jumlahPenjaga;
    PenjagaPerLevel(level, daftarPenjaga, jumlahPenjaga);

    p1.x = 2; p1.y = PLAY_H-2; p1.status=0;
    p2.x = PLAY_W-3; p2.y = PLAY_H-2; p2.status=0;

    time_t start = time(NULL);
    waktuBermain = 0;
    
    bool tertangkap = false;
    bool finished = false;

    int mulaiX = (COLS - PLAY_W) / 2;
    int mulaiY = (LINES - PLAY_H) / 2;
    
    WINDOW *header  = newwin(4, COLS, 0, 0);
    WINDOW *playwin = newwin(PLAY_H, PLAY_W, mulaiY, mulaiX);
    
    keypad(stdscr, TRUE);
    timeout(0);
    
    while (true) {
        int ch = getch();
        if (ch != ERR) {
            if (ch == KEY_UP && p1.y>0) p1.y--;
            else if (ch == KEY_DOWN && p1.y<PLAY_H-2) p1.y++;
            else if (ch == KEY_LEFT && p1.x>0) p1.x--;
            else if (ch == KEY_RIGHT && p1.x<PLAY_W-2) p1.x++;
            else if (ch=='w'||ch=='W') { if(p2.y>0) p2.y--; }
            else if (ch=='s'||ch=='S') { if(p2.y<PLAY_H-2) p2.y++; }
            else if (ch=='a'||ch=='A') { if(p2.x>0) p2.x--; }
            else if (ch=='d'||ch=='D') { if(p2.x<PLAY_W-2) p2.x++; }
            else if (ch=='q'||ch=='Q') {tertangkap=true; p1.status=1;p2.status=1; break;}
        } 
        
        for (int i = 0; i < jumlahPenjaga; i++) {
            daftarPenjaga[i].tick++;
            if(daftarPenjaga[i].tick>=daftarPenjaga[i].speed){
                daftarPenjaga[i].tick=0;
                daftarPenjaga[i].x+=daftarPenjaga[i].dir;
                if(daftarPenjaga[i].x<=0) daftarPenjaga[i].x=PLAY_W-3;
                if(daftarPenjaga[i].x>=PLAY_W-2) daftarPenjaga[i].x=1;
            }
        }
        
        for(int i = 0; i < jumlahPenjaga; i++){
            if(!daftarPenjaga[i].active) continue;
            if(daftarPenjaga[i].x==p1.x && daftarPenjaga[i].y==p1.y) {
			p1.status=1; 
			tertangkap=true;
        }
			if(daftarPenjaga[i].x==p2.x && daftarPenjaga[i].y==p2.y){
			p2.status=1;
			tertangkap=true;
        }
    }
    
        if(p1.y<=0)p1.status=2;
        if(p2.y<=0)p2.status=2;

        waktuBermain = (int)difftime(time(NULL), start);
        areaPermainan(playwin,p1,p2,daftarPenjaga,jumlahPenjaga);
        mvprintw(mulaiY + PLAY_H, mulaiX, "Controls: ");
        mvprintw(mulaiY + PLAY_H + 1, mulaiX,"P1 = Arrow keys  P2 = WASD");
        mvprintw(mulaiY + PLAY_H + 2, mulaiX,"q to quit");
	
		werase(header);
        mvwprintw(header, 1, 2, "Level %d", level);
        mvwprintw(header, 2, 2, "P1: %s %s", p1.name, p1.status==0?"Bermain":p1.status==1?"Tertangkap":"Finish");
        mvwprintw(header, 3, 2, "P2: %s %s", p2.name, p2.status==0?"Bermain":p2.status==1?"Tertangkap":"Finish");
        mvwprintw(header, 1, COLS-20, "Waktu: %s", formatWaktu(waktuBermain).c_str());
        wrefresh(header); 
        
        if(tertangkap){
			Sleep(700);
			break;
		}
        if(p1.status==2 && p2.status==2) {finished=true; break;}
        napms(TICK_MS);
    }
    
    timeout(-1);
        
    werase(playwin);    
    wrefresh(playwin);
    delwin(playwin);
    
    werase(header);
    wrefresh(header); 
    delwin(header);


    if(finished){
        if(skorMain[level]==0 || waktuBermain<skorMain[level]){
           skorMain[level]=waktuBermain;
            simpanSkor(level, waktuBermain);
        }
        return true;
    }
    return false;
}

int main(){
    Player p1, p2;
    init_ncurses();  

    opening();       
    namaPlayer(p1, p2);  
    informasi();     

    while (true) {
    	erase();
        refresh();

        menu(); 
        timeout(-1);  
        int level = menuLevel();  
        timeout(0);    
        if (level == -1) break;   

        int durasiLevel;
        bool menang = bermain(level, p1, p2, durasiLevel);
        if (menang && level == levelMaksTersedia) {
            levelMaksTersedia++;
        if (levelMaksTersedia > totalLevel)
            levelMaksTersedia = totalLevel;
}

        clear();
        if (menang) {
        	attron(COLOR_PAIR(3));
            mvprintw(LINES / 2, (COLS - 21) / 2, "Selamat! Kamu Menang!");
            mvprintw(LINES / 2 + 1, (COLS - 20) / 2, "Waktu bermain: %s", formatWaktu(durasiLevel).c_str());
            attroff(COLOR_PAIR(3));
        } else {
        	attron(COLOR_PAIR(1));
            mvprintw(LINES / 2, (COLS - 11) / 2, "Kamu Kalah!");
            mvprintw(LINES / 2 + 1, (COLS - 20) / 2, "Waktu bermain: %s", formatWaktu(durasiLevel).c_str());
            attroff(COLOR_PAIR(1));
        }
        timeout(-1);
        while (getch() != '\n'); 
        timeout(0);
    }
    
    endwin();
    return 0;
}
