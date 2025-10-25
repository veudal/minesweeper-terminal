// minesweeper.c - Terminal Minesweeper
// Compile: gcc minesweeper.c -lncursesw -o ms
// Run: ./ms 20  or ./ms 20 20
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
    int mine;
    int revealed;
    int flagged;
    int adjacent;
} Cell;

int rows = 24, cols = 24;
int mines = 0;
Cell *board = NULL;
int cursor_r = 0, cursor_c = 0;
int game_over = 0;
int won = 0;
int revealed_count = 0;
int first_move = 1;
int cleared_percent = 0;

double elapsed = 0.0;
struct timeval start_time;
int timer_started = 0;

#define IDX(r, c) ((r) * cols + (c))

int in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < cols; }

void place_mines(int first_r, int first_c) {
    int max_mines = rows * cols / 6;
    if (mines <= 0) mines = max_mines;
    srand((unsigned)time(NULL));
    int placed = 0;
    while (placed < mines) {
        int r = rand() % rows;
        int c = rand() % cols;
        if (board[IDX(r, c)].mine) continue;
        if (abs(r - first_r) <= 1 && abs(c - first_c) <= 1) continue;
        board[IDX(r, c)].mine = 1;
        placed++;
    }
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            if (board[IDX(r, c)].mine) { board[IDX(r, c)].adjacent = -1; continue; }
            int cnt = 0;
            for (int dr=-1; dr<=1; ++dr)
                for (int dc=-1; dc<=1; ++dc)
                    if ((dr||dc) && in_bounds(r+dr,c+dc) && board[IDX(r+dr,c+dc)].mine)
                        cnt++;
            board[IDX(r,c)].adjacent = cnt;
        }
}

void flood_reveal(int r,int c){
    int stack_size=rows*cols*2;
    int *stack=malloc(stack_size*sizeof(int));
    int sp=0;
    stack[sp++] = r; stack[sp++] = c;
    while(sp>0){
        int cc=stack[--sp]; int rr=stack[--sp];
        if(!in_bounds(rr,cc)) continue;
        Cell *cell=&board[IDX(rr,cc)];
        if(cell->revealed || cell->flagged) continue;
        cell->revealed=1; revealed_count++;
        if(cell->adjacent==0){
            for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++)
                if(dr||dc){ stack[sp++]=rr+dr; stack[sp++]=cc+dc; }
        }
    }
    free(stack);
}

void reveal_cell(int r,int c);
void auto_flag_and_chord(int r,int c,int user){
    if(!in_bounds(r,c)) return;
    Cell *cell=&board[IDX(r,c)];
    if(!cell->revealed || cell->adjacent<0) return;
    int flagged=0,hidden=0;
    for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++)
        if(in_bounds(r+dr,c+dc) && (dr||dc)){
            if(board[IDX(r+dr,c+dc)].flagged) flagged++;
            if(!board[IDX(r+dr,c+dc)].revealed && !board[IDX(r+dr,c+dc)].flagged) hidden++;
        }
    if(flagged==cell->adjacent){
        for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++)
            if(in_bounds(r+dr,c+dc) && (dr||dc)){
                Cell *nb=&board[IDX(r+dr,c+dc)];
                if(!nb->revealed && !nb->flagged) reveal_cell(r+dr,c+dc);
            }
    }
    if(!user) return;
    if(hidden>0 && hidden+flagged==cell->adjacent){
        for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++)
            if(in_bounds(r+dr,c+dc) && (dr||dc)){
                Cell *nb=&board[IDX(r+dr,c+dc)];
                if(!nb->revealed && !nb->flagged) nb->flagged=1;
            }
    }
}

void reveal_cell(int r,int c){
    if(!in_bounds(r,c)) return;
    Cell *cell=&board[IDX(r,c)];
    if(cell->revealed || cell->flagged) return;
    if(cell->mine){ cell->revealed=1; game_over=1; won=0; return; }
    cell->revealed=1; revealed_count++;
    if(cell->adjacent==0) flood_reveal(r,c);
    auto_flag_and_chord(r,c,0);
}

void check_win(){
    int total=rows*cols; int non_mine=total-mines;
    if(revealed_count>=non_mine){ game_over=1; won=1; }
}

void reveal_all_mines(){ for(int i=0;i<rows*cols;i++) if(board[i].mine) board[i].revealed=1; }

void reset_game(){
    free(board);
    board=calloc(rows*cols,sizeof(Cell));
    cursor_r=cursor_c=0;
    game_over=0; won=0; revealed_count=0; first_move=1;
    timer_started=0;
    elapsed = 0;
    clear();
}

void draw_board(){
    static int prev_h=0,prev_w=0,prev_y=-1,prev_x=-1;
    int term_h,term_w; getmaxyx(stdscr,term_h,term_w);
    int board_w=cols*2+2, board_h=rows+2;
    int start_y=(term_h-board_h)/2, start_x=(term_w-board_w)/2;
    if(start_y<0) start_y=0; if(start_x<0) start_x=0;
    if(term_h!=prev_h || term_w!=prev_w || start_y!=prev_y || start_x!=prev_x){ clear(); prev_h=term_h; prev_w=term_w; prev_y=start_y; prev_x=start_x; }
    box(stdscr,0,0);

    int flagged=0,cleared=0;
    for(int i=0;i<rows*cols;i++){ if(board[i].flagged) flagged++; if(board[i].revealed) cleared++; }
    if(!game_over)
        cleared_percent = (rows*cols > 0)? (cleared * 100 / (rows * cols - mines)) : 0;


    if(timer_started && !game_over){
        struct timeval now; gettimeofday(&now,NULL);
        elapsed=(now.tv_sec-start_time.tv_sec)+(now.tv_usec-start_time.tv_usec)/1e6;
    }

    mvprintw(1,2,"Minesweeper %dx%d | Flags: %d/%d | Cleared: %d%% | Time: %.2f s    ",
             rows,cols,flagged,mines,cleared_percent,elapsed);

    for(int r=0;r<=rows+1;r++) for(int c=0;c<=cols+1;c++){
        int y=start_y+r, x=start_x+c*2;
        if(r==0||r==rows+1) mvaddwstr(y,x,L"──");
        else if(c==0) mvaddwstr(y,x,L"│");
        else if(c==cols+1) mvaddwstr(y,x+1,L"│");
    }
    mvaddwstr(start_y,start_x,L"┌");
    mvaddwstr(start_y,start_x+(cols+1)*2+1,L"┐");
    mvaddwstr(start_y+rows+1,start_x,L"└");
    mvaddwstr(start_y+rows+1,start_x+(cols+1)*2+1,L"┘");

    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++){
            int y=start_y+r+1, x=start_x+c*2+2;
            Cell *cell=&board[IDX(r,c)];
            bool selected=(r==cursor_r && c==cursor_c);
            if(selected && cell->revealed) attron(A_REVERSE);

            if(!cell->revealed){
                if(cell->flagged){
                    if(selected){ attron(COLOR_PAIR(12)|A_BOLD); mvaddwstr(y,x,L"⚑ "); attroff(COLOR_PAIR(12)|A_BOLD); }
                    else { attron(COLOR_PAIR(9)|A_BOLD); mvaddwstr(y,x,L"⚑ "); attroff(COLOR_PAIR(9)|A_BOLD); }
                } else {
                    if(selected){ attron(COLOR_PAIR(11)); mvaddwstr(y,x,L"  "); attroff(COLOR_PAIR(11)); }
                    else { attron(COLOR_PAIR(5)); mvaddwstr(y,x,L"██"); attroff(COLOR_PAIR(5)); }
                }
            } else if(cell->mine){ attron(COLOR_PAIR(10)|A_BOLD); mvaddwstr(y,x,L"💣"); attroff(COLOR_PAIR(10)|A_BOLD); }
            else { int adj=cell->adjacent; if(adj==0) mvaddwstr(y,x,L"  "); else { attron(COLOR_PAIR(adj)|A_BOLD); mvprintw(y,x,"%d ",adj); attroff(COLOR_PAIR(adj)|A_BOLD); } }

            if(selected && cell->revealed) attroff(A_REVERSE);
        }
    }

    if(game_over){
        char msg[100];
        if(won) {
            snprintf(msg,sizeof(msg),"🎉 YOU WON in %.2f seconds! Press 'q' to quit or 'r' to restart", elapsed); cleared_percent = 100;
        }
        else snprintf(msg,sizeof(msg),"💥 BOOM! You lost. Press 'q' to quit or 'r' to restart");
        int msg_len=strlen(msg);
        attron(won?(COLOR_PAIR(2)|A_BOLD):(COLOR_PAIR(3)|A_BOLD));
        mvprintw(start_y+rows+3,(term_w-msg_len)/2,"%s",msg);
        attroff(won?(COLOR_PAIR(2)|A_BOLD):(COLOR_PAIR(3)|A_BOLD));
    }

    refresh();
}

int main(int argc,char **argv){
    setlocale(LC_ALL, "C.UTF-8");
    int mine_percent=15;
    if(argc>=2){int a=atoi(argv[1]); if(a>0) rows=cols=a;}
    if(argc>=3){int a=atoi(argv[1]),b=atoi(argv[2]); if(a>0) rows=a; if(b>0) cols=b;}
    if(argc>=4){int p=atoi(argv[3]); if(p>0 && p<=95) mine_percent=p;}
    mines=mine_percent*(rows*cols/100);

    board=calloc(rows*cols,sizeof(Cell));

    initscr(); cbreak(); noecho(); keypad(stdscr,TRUE); curs_set(0); nodelay(stdscr,TRUE);
    if(has_colors()){ start_color(); use_default_colors();
        init_pair(1,COLOR_BLUE,-1); init_pair(2,COLOR_GREEN,-1); init_pair(3,COLOR_RED,-1);
        init_pair(4,COLOR_MAGENTA,-1); init_pair(5,COLOR_CYAN,-1); init_pair(6,COLOR_YELLOW,-1);
        init_pair(7,COLOR_WHITE,-1); init_pair(8,COLOR_BLACK,-1); init_pair(9,COLOR_YELLOW,-1);
        init_pair(10,COLOR_RED,-1); init_pair(11,COLOR_BLACK,COLOR_WHITE); init_pair(12,COLOR_RED,COLOR_WHITE);
    }

    draw_board();
    while(1){
        int ch=getch();
        if(ch=='q'||ch=='Q') break;

        if(ch=='r'||ch=='R'){ reset_game(); draw_board(); continue; }

        if(!game_over){
            Cell *cell=&board[IDX(cursor_r,cursor_c)];
            if(ch==KEY_UP||ch=='w'||ch=='W'){if(cursor_r>0) cursor_r--;}
            else if(ch==KEY_DOWN||ch=='s'||ch=='S'){if(cursor_r<rows-1) cursor_r++;}
            else if(ch==KEY_LEFT||ch=='a'||ch=='A'){if(cursor_c>0) cursor_c--;}
            else if(ch==KEY_RIGHT||ch=='d'||ch=='D'){if(cursor_c<cols-1) cursor_c++;}
            else if(ch=='f'||ch=='F'||ch=='j'||ch=='J'){if(!cell->revealed) cell->flagged=!cell->flagged;}
            else if(ch==' '||ch=='k'||ch=='K'){
                if(!timer_started) gettimeofday(&start_time,NULL), timer_started=1;
                if(first_move) place_mines(cursor_r,cursor_c), first_move=0;

                if(cell->revealed) auto_flag_and_chord(cursor_r,cursor_c,1);
                else reveal_cell(cursor_r,cursor_c);

                if(game_over) reveal_all_mines();
            }
            check_win();
            if(game_over && !won) reveal_all_mines();
        }

        draw_board();
        usleep(50000); // ~20 FPS refresh
    }

    endwin();
    free(board);
    return 0;
}

