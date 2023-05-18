/*
 * Student ID : 2015726091
 * Name : Kwon Sunho
 */
#include <ncurses.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define MAX_LQ_SIZE 5
#define MAX 20
#define PERMS 0644

struct __User{
    char name[MAX + 1];
    char pw[MAX + 1];
    int curenter;
    int win;
    int lose;
    char stone;
    int stoneNum;
    int turncnt;
};
typedef struct __User User;

void signalHandler(int signum);
void errorHandler(const char * msg);
void waitingPlayer(WINDOW* win, WINDOW * win2);

void main_before_login();
void main_after_login();
void sign_up();
void sign_in();
void withdrawal();
void playerInfomation();
void ingamepage();
User* inputdata(WINDOW* win);

void pageHandler();
int rlHandler(WINDOW * win);
int sign_page_cur_Handler(WINDOW* win,WINDOW* win2, User* nowuser);
int sign_in_cur_Handler(WINDOW* win, WINDOW * win2, User* nowuser);
int before_play_enter(WINDOW * win);
int with_cur_handler(WINDOW* win, WINDOW * win2, User* nowuser);
int ingamehandler(WINDOW* win, WINDOW* win2);
int ingamepagehandler(WINDOW * win, WINDOW * win2, int ox, int oy);
void printBrdValue(char(*)[6], WINDOW* win, WINDOW * win2);
void playGame(WINDOW* win , WINDOW* win2);
int stonechecker(int, int);
int bordcal(int, int);

char *before_login[] = {"", "SIGN IN", "SIGN UP", "EXIT"};
char *before_play[] = {"", "PLAY", "SIGN OUT", "WITHDRAWAL"};
char *sign_in_menu[] = {"", "SIGN IN" , "BACK"};
char *sign_up_menu[] = {"", "SIGN UP" , "BACK"};
char *with_mes[] = {"", "WITHDRAWAL", "BACK"};

char initbordvalue[6][6] = { {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', 'O', 'X', ' ', ' ' },
                         {' ', ' ', 'X', 'O', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' }
                        };

char bordvalue[6][6] = { {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', 'O', 'X', ' ', ' ' },
                         {' ', ' ', 'X', 'O', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' }
                        };

int sockfd = 0;
int clntsock = 0;
int curenter = 0;
int y =0 , x =0; 
int ch = 0, id = 0 ;
int win = 0, lose = 0;
double per = 0;
int point[2];
int totalStn = 4;

User user;
User* nowvalue;
User enemy;

int main(int argc, char const *argv[]) {
    struct sockaddr_in servAddr = { 0x00, };  // 서버의 주소 정보
    struct sockaddr_in clntAddr = { 0x00, };  // 클라이언트의 주소 정보
    int clntAddrSize = 0;

    signal(SIGINT, signalHandler);
    signal(SIGPIPE, signalHandler);
    
    if (argc != 3) {
        printf("Usage : %s [server IP] [port]\n", argv[0]);
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        errorHandler("sock() error!");
    }
    
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
        errorHandler("connect() error!");
    }

    pageHandler();
    return 0;
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        close(sockfd);
        exit(-1);
    }
    if (signum == SIGPIPE) {
        puts("disconnected!");
        exit(-1);
    }
}

void errorHandler(const char* msg){
    close(sockfd);
    fputs(msg, stderr);
    exit(-1);
}

void printBrdValue(char(*bord)[6], WINDOW * win, WINDOW * win2) {
    int rx = 18; int cy = 6; int ix; int jy;
    wmove(win, 6, 18);
    for( ix = 0; ix < 6; ix ++){
        cy = 6;
        for( jy = 0; jy < 6; jy++){
            mvwprintw(win, cy, rx, "%c", bordvalue[jy][ix]);
            cy+=2;
        }
        rx += 4;
    }

    if( user.turncnt == 1){
        if( enemy.stone == 1 ){
            mvwprintw(win2, 8, 2, "%s(O)" , enemy.name);
            wprintw(win2, " : %d", (enemy.stoneNum));
            wattron(win2, A_REVERSE);
            mvwprintw(win2, 9, 2, "%s(X)" , user.name);
            wattroff(win2, A_REVERSE);
            wprintw(win2, " : %d", user.stoneNum);
        }
        else{
            wattron(win2, A_REVERSE);
            mvwprintw(win2, 8, 2, "%s(O)" , user.name);
            wattroff(win2, A_REVERSE);
            wprintw(win2, " : %d", user.stoneNum);
            mvwprintw(win2, 9, 2, "%s(X) : %d" , enemy.name, (enemy.stoneNum));
        }
    }
    else{
        if( enemy.stone == 1 ){
            wattron(win2, A_REVERSE);
            mvwprintw(win2, 8, 2, "%s(O)" , enemy.name);
            wattroff(win2, A_REVERSE);
            wprintw(win2, " : %d", (enemy.stoneNum));
            mvwprintw(win2, 9, 2, "%s(X) : %d" , user.name , user.stoneNum);
        }
        else{
            mvwprintw(win2, 8, 2, "%s(O) : %d" , user.name, user.stoneNum);
            wattron(win2, A_REVERSE);
            mvwprintw(win2, 9, 2, "%s(X)" , enemy.name);
            wattroff(win2, A_REVERSE);
            wprintw(win2, " : %d", (enemy.stoneNum) );
        }
    }

}

//페이지 핸들러
void pageHandler(){
    main_before_login();
    while(1){
        switch(curenter){
            case 7 :
                main_before_login();
                endwin();
                break;
            case 8 :
                sign_in();
                endwin();
                break;
            case 9 :
                playerInfomation();
                endwin();
                break;
            case 11 :
                main_after_login();
                endwin();
                break;
            case 16 :
                main_after_login();
                endwin();
                break;
            case 35 :
                sign_up();
                endwin();
                break;
            case 36 :
                user.curenter = curenter;
                send(sockfd, &user, sizeof(User), 0);
                main_before_login();
                endwin();
                break;
            case 10 : case 60 : case 61 :
                main_before_login();
                endwin();
                break;
            case 42 :
                user.curenter = 42;
                send(sockfd, &user, sizeof(User), 0);
                recv(sockfd, &bordvalue ,sizeof(char[6][6]), 0);
                ingamepage();
                curenter = 16;
                endwin();
                break;
            case 62 :
                endwin(); 
                break;
            case 63 :
                withdrawal();
                endwin();
                break;
            case 65 : 
                main_after_login();
                endwin();
                break;
            case 101:
                main_after_login();
                endwin();
                break;
            default:
                return;
        }
        if ( curenter == 62 ){
            user.curenter = 62;
            send(sockfd, &user, sizeof(User), 0);
            break;
        }
    }
}

//커서 핸들러
int rlHandler(WINDOW * win){
    getyx(win , y , x);
    int xx= x;
    int i =1;
    curs_set(0);
    while(1){
        noecho();
        ch = wgetch(win);
        if(ch == 10){
            break;
        }
        switch(ch){
            case KEY_RIGHT :
                wattrset(win,A_NORMAL);
                if ( x % 3 == 0)
                    mvwprintw(win, 3, x, before_play[i]);
                else
                    mvwprintw(win, 3, x, "%s", before_login[i]);
                x+=27;
                i++;
                if( x > 80){
                    x = xx; i = 1;
                }

                wmove(win, y, x);
                wattron(win, A_REVERSE);
                if ( x%3 == 0)
                    mvwprintw(win, 3, x, before_play[i]);
                else
                    mvwprintw(win, 3, x, "%s", before_login[i]);
                wattroff(win, A_REVERSE);
                break;
            case KEY_LEFT :
                wattrset(win,A_NORMAL);
                if(x%3 == 0)
                    mvwprintw(win, 3, x, before_play[i]);
                else
                    mvwprintw(win, 3, x, "%s", before_login[i]);
                x-=27;
                i--;
                if(x < 8){
                    x = xx+54;   i = 3;
                }

                wmove(win , y , x);
                wattron(win, A_REVERSE);
                if(x%3 == 0)
                    mvwprintw(win, 3, x, before_play[i]);
                else
                    mvwprintw(win, 3, x, "%s", before_login[i]);
                wattroff(win, A_REVERSE);
                break;   
            case 10 :
                wmove(win,y,x);
                break;
            default :
                wmove(win, y, x);
        }
     
    }
    return x;
}

int sign_page_cur_Handler(WINDOW* win,WINDOW* win2, User* nowuser) {
    int udy, udx;
    getyx(win , udy , udx);
    int i = 1; int fi = udx;
    while(1){
        noecho();
        wmove(win, udy, udx);
        fflush(stdin);
        ch = wgetch(win);
        switch(ch){
            case KEY_RIGHT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", sign_up_menu[i]);
                udx += 50;
                i++;
                if(udx  > 80){
                    udx = fi;  i = 1;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, sign_up_menu[i]);
                wattroff(win, A_REVERSE);
                break;

            case KEY_LEFT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", sign_up_menu[i]);
                udx-=50;
                i--;
                if(udx < 8){
                    udx = fi+50; i = 2;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, sign_up_menu[i]);
                wattroff(win, A_REVERSE);
                break;
            default :
                wmove(win, udy, udx);
        }
        if ( ch == 10 && udx == fi ){
            nowuser->curenter = fi;
            send(sockfd, nowuser, sizeof(User), 0);
            recv(sockfd, nowuser, sizeof(User), 0);
            id = nowuser->curenter;
            if( id == 1){
                mvwprintw(win, 5, 1, "\t\t\t\t\t\t\t\t\t\t");
                mvwprintw(win, 5, 1, ">>> Welcome to OTHELLO World! (Press any key...)");
                wrefresh(win);
                getch();
                return udx;
            }
            else{
                mvwprintw(win, 5,1, ">>> %s has already exist in DB! (Press any key...)", nowuser->name);
                mvwprintw(win2, 7, 32, "\t\t\t\t\t\t\t\t\t");
                mvwprintw(win2, 9, 32, "\t\t\t\t\t\t\t\t\t");
                wrefresh(win2);
                wrefresh(win);
                getch();
                fflush(stdin);
                nowuser = inputdata(win2);
            }
        }
        else if ( ch == 10 && udx != fi){
            break;
        }

    }
    return udx;
}

int sign_in_cur_Handler(WINDOW* win, WINDOW * win2, User* nowuser) {
    int udy, udx;
    getyx(win , udy , udx);
    int i = 1; int fi = udx;
    while(1){
        noecho();
        wmove(win, udy, udx);
        fflush(stdin);
        ch = wgetch(win);
        switch(ch){
            case KEY_RIGHT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", sign_in_menu[i]);
                udx += 50;
                i++;
                if(udx  > 80){
                    udx = fi;  i = 1;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, sign_in_menu[i]);
                wattroff(win, A_REVERSE);
                break;

            case KEY_LEFT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", sign_in_menu[i]);
                udx-=50;
                i--;
                if(udx < 8){
                    udx = fi+50; i = 2;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, sign_in_menu[i]);
                wattroff(win, A_REVERSE);
                break;
            default :
                wmove(win, udy, udx);
        }
        if ( ch == 10 && udx == fi){
            nowuser->curenter = fi;
            send(sockfd, nowuser, sizeof(User), 0);
            recv(sockfd, nowuser, sizeof(User), 0);
            
            id = nowuser->curenter;

            if( id == 1){
                mvwprintw(win, 5, 1, "\t\t\t\t\t\t\t\t\t\t");
                mvwprintw(win, 5, 1, ">>> Welcome to OTHELLO World! (Press any key...)");
                wrefresh(win);
                getch();
                strcpy (user.name ,nowuser->name);
                strcpy (user.pw , nowuser->pw);
                return udx;
            }
            else if( id == 3){
                mvwprintw(win, 5,1, ">>> It is already in game. try again!");
                wrefresh(win);
                getch();
                fflush(stdin);
                mvwprintw(win2, 7, 32, "\t\t\t\t\t\t\t\t");
                mvwprintw(win2, 9, 32, "\t\t\t\t\t\t\t\t");
                wrefresh(win2);

                nowuser = inputdata(win2);
            }
            else{
                mvwprintw(win, 5,1, ">>> ID or Passward are wrong. try again!");
                wrefresh(win);
                getch();
                fflush(stdin);
                mvwprintw(win2, 7, 32, "\t\t\t\t\t\t\t\t");
                mvwprintw(win2, 9, 32, "\t\t\t\t\t\t\t\t");
                wrefresh(win2);

                nowuser = inputdata(win2);
            }
        }
        else if ( ch == 10 && udx != fi){
            break;
        }
    }
    return udx;
}

int before_play_enter(WINDOW * win){
    getyx(win, y, x);
    curs_set(0);
    while(1){
        noecho();
        ch = wgetch(win);
        if( ch == 10)
            break;
        switch(ch){
            case KEY_RIGHT: case KEY_LEFT: case KEY_UP : case KEY_DOWN :
                wmove(win, y, x);
            break;
            default :
                wmove(win, y, x);
        }
    }
    return x;
}

int with_cur_handler(WINDOW* win, WINDOW * win2, User* nowuser){
    int udy, udx;
    getyx(win , udy , udx);
    int i = 1; int fi = udx;
    while(1){
        noecho();
        wmove(win, udy, udx);
        fflush(stdin);
        ch = wgetch(win);
        switch(ch){
            case KEY_RIGHT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", with_mes[i]);
                udx += 58;
                i++;
                if(udx  > 80){
                    udx = fi;  i = 1;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, with_mes[i]);
                wattroff(win, A_REVERSE);
                break;

            case KEY_LEFT :
                wattrset(win,A_NORMAL);
                mvwprintw(win, 3, udx, "%s", with_mes[i]);
                udx-=58;
                i--;
                if(udx < 8){
                    udx = fi+58; i = 2;
                }
                wmove(win, udy, udx);
                wattron(win, A_REVERSE);
                mvwprintw(win, 3, udx, with_mes[i]);
                wattroff(win, A_REVERSE);
                break;
            case 10 :
                wmove(win,y,x);
                break;
            default :
                wmove(win, udy, udx);
        }
        if ( ch == 10 && udx == fi){
            nowuser->curenter = fi;
            send(sockfd, nowuser, sizeof(User), 0);
            recv(sockfd, &id, sizeof(int), 0);

            if( id == 1){
                mvwprintw(win, 5, 1, "\t\t\t\t\t\t\t\t\t");
                mvwprintw(win, 5, 1, ">>> Goodbye Player (Press any key...)");
                wrefresh(win);
                getch();
                return udx;
            }
            else{
                mvwprintw(win, 7, 32, "\t\t\t\t\t\t\t\t");
                mvwprintw(win, 9, 32, "\t\t\t\t\t\t\t\t");
                wrefresh(win);
                mvwprintw(win, 5, 1, ">>> It does not exist!");
                wrefresh(win2);

                nowuser = inputdata(win2);
            }
        }
        else if ( ch == 10 && udx != fi)
            break;
    }
    return udx;
}

int ingamehandler(WINDOW* win, WINDOW* win2){
    printBrdValue( bordvalue,  win, win2);
    wrefresh(win);
    wrefresh(win2);
    wmove(win, 10, 26);
    int px, py;
    int dx=2, dy=2;
    int cc;
    getyx(win, py, px);
    while(1){
        noecho();
        ch = wgetch(win);
        
        switch(ch){
            case KEY_RIGHT: 
                wattrset(win,A_NORMAL);
                mvwprintw(win, py, px , "%c", bordvalue[dy][dx]);
                px+=4;
                dx++;
                if( px > 38){
                    if( py == 16){
                        py = 6; px = 18;
                        dx = 0; dy = 0;
                    }
                    else{
                        py +=2; px = 18;
                        dx = 0; dy++;
                    }
                }

                wmove(win, py, px);
                wattron(win, A_REVERSE);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wattroff(win, A_REVERSE);
                break;
            case KEY_LEFT: 
                wattrset(win,A_NORMAL);
                mvwprintw(win, py, px , "%c", bordvalue[dy][dx] );
                px-=4;
                dx--;
                if( px < 18){
                    if( py == 6){
                        py = 16; px = 38;
                        dx = 5; dy = 5;
                    }
                    else{
                        py-=2; px = 38;
                        dx= 5; dy--;
                    }
                }

                wmove(win, py, px);
                wattron(win, A_REVERSE);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wattroff(win, A_REVERSE);
                break;
            case KEY_UP : 
                wattrset(win,A_NORMAL);
                mvwprintw(win, py, px , "%c", bordvalue[dy][dx]);
                py-=2;
                dy--;
                if( py < 6){
                    if( px == 38){
                        py = 16; px = 18;
                        dx = 5; dy = 5;
                    }
                    else{
                        py = 16; px += 4;
                        dx++; dy = 5;
                    }
                }
                wmove(win, py, px);
                wattron(win, A_REVERSE);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wattroff(win, A_REVERSE);
                break;                
            case KEY_DOWN :
                wattrset(win,A_NORMAL);
                mvwprintw(win, py, px , "%c", bordvalue[dy][dx]);
                py+=2;
                dy++;
                if( py > 16){
                    if( px == 18){
                        py = 6; px = 38;
                        dx = 5; dy = 0;
                    }
                    else{
                        py = 6; px -= 4;
                        dx--; dy = 0;
                    }
                }
                wmove(win, py, px);
                wattron(win, A_REVERSE);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wattroff(win, A_REVERSE);
                break; 
            case 110:
                wattrset(win, A_NORMAL);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wmove(win2, 12, 7);
                ch = ingamepagehandler(win, win2, px, py);
                break;
            case 114:
                wattrset(win, A_NORMAL);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wmove(win2, 14, 8);
                ch = ingamepagehandler(win, win2, px, py);
                break;
            case 120 :
                wattrset(win, A_NORMAL);
                mvwprintw(win, py, px, "%c", bordvalue[dy][dx]);
                wmove(win2, 16, 8);
                ch = ingamepagehandler(win, win2, px, py);
                break;
            case 10:
                cc = stonechecker(dx, dy);
                wrefresh(win);
                wrefresh(win2);
                if ( cc > 0){
                    point[0] = dy; point[1] = dx;
                    send(sockfd, &point, sizeof(int[2]), 0);
                    recv(sockfd, &bordvalue , sizeof( char[6][6]), 0);
                
                    printBrdValue( bordvalue,  win, win2);
                    break;
                }
                else
                    break;
            default :
                wmove(win, py, px);
        }
        if ( ch == 12){
            point[0] = -1; point[1] = -1;
            send(sockfd, &point, sizeof(int[2]), 0);
            recv(sockfd, &bordvalue, sizeof(char[6][6]), 0);
            break;
        }
        else if ( ch == 16){
            return 10;
        }
        if( ch == 10 && cc > 0){
            break;
        }
        if ( ch == 103){
            wattrset(win2,A_NORMAL);
            mvwprintw(win2,16, 8, "E");
            waddch(win2, 'X' | A_UNDERLINE);
            wprintw(win2, "IT");
            mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
            wprintw(win2, "EGAME");
            mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );                
            wprintw(win2, "EXT TURN");
            wattron(win, A_REVERSE);
            wmove(win, py, px);
            wattroff(win, A_REVERSE);
        }
        else if ( ch == 16 )
            return 16; 

    }
    return px;
}

int ingamepagehandler(WINDOW * win, WINDOW * win2, int ox, int oy){
    int px, py;
    getyx(win2, py, px);
    if( py == 16){
        wattron(win2, A_REVERSE);
        mvwprintw(win2,16, 8, "E");
        waddch(win2, 'X' | A_UNDERLINE);
        wprintw(win2, "IT");
        wattroff(win2, A_REVERSE);
   }
   else if ( py == 14){
        wattron(win2, A_REVERSE);
        mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
        wprintw(win2, "EGAME");
        wattroff(win2, A_REVERSE);
   }
   else if ( py == 12){
        wattron(win2, A_REVERSE);
        mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
        wprintw(win2, "EXT TURN");
        wattroff(win2, A_REVERSE);
   }
    while(1){
        noecho();
        ch = wgetch(win2);
        if( ch == 10)
            break;
        switch(ch){
            case KEY_DOWN : 
                wattrset(win,A_NORMAL);
                mvwprintw(win2,16, 8, "E");
                waddch(win2, 'X' | A_UNDERLINE);
                wprintw(win2, "IT");
                mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                wprintw(win2, "EGAME");
                mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                wprintw(win2, "EXT TURN");
                py+=2;
                if(py > 16){
                    py = 12;
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                    wprintw(win2, "EXT TURN");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 14){
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                    wprintw(win2, "EGAME");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 16){
                    wattron(win2, A_REVERSE);
                    mvwprintw(win2,16, 8, "E");
                    waddch(win2, 'X' | A_UNDERLINE);
                    wprintw(win2, "IT");
                    wattroff(win2, A_REVERSE);
                }
                break;   
            case KEY_UP :
                wattrset(win,A_NORMAL);
                mvwprintw(win2,16, 8, "E");
                waddch(win2, 'X' | A_UNDERLINE);
                wprintw(win2, "IT");
                mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                wprintw(win2, "EGAME");
                mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                wprintw(win2, "EXT TURN");
                py-=2;
                if(py < 12){
                    py = 16;       
                    wattron(win2, A_REVERSE);
                    mvwprintw(win2,16, 8, "E");
                    waddch(win2, 'X' | A_UNDERLINE);
                    wprintw(win2, "IT");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 14){
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                    wprintw(win2, "EGAME");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 12){
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                    wprintw(win2, "EXT TURN");
                    wattroff(win2, A_REVERSE);
                }
                break;
            case KEY_LEFT : case KEY_RIGHT : 
                if( py == 16){
                    wattron(win2, A_REVERSE);
                    mvwprintw(win2,16, 8, "E");
                    waddch(win2, 'X' | A_UNDERLINE);
                    wprintw(win2, "IT");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 14){
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                    wprintw(win2, "EGAME");
                    wattroff(win2, A_REVERSE);
                }
                else if ( py == 12){
                    wattron(win2, A_REVERSE);
                    mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                    wprintw(win2, "EXT TURN");
                    wattroff(win2, A_REVERSE);
                }
                break;
            case 103 :
                wattrset(win,A_NORMAL);
                mvwprintw(win2,16, 8, "E");
                mvwaddch(win2, 16, 9, 'X' | A_UNDERLINE);
                wprintw(win2, "IT");
                mvwaddch(win2, 14, 8, 'R' | A_UNDERLINE);
                wprintw(win2, "EGAME");
                mvwaddch(win2, 12, 7, 'N'|A_UNDERLINE );
                wprintw(win2, "EXT TURN");
                getyx(win, oy, ox);
                wmove (win, oy, ox);
                break;
            default :
                wmove(win2, py, px);    
        }
        if( ch == 103)
            break;
    }
    if( ch == 10)
        return py;
    else if ( ch == 103){
        return 103;
    }
}

int stonechecker(int px, int py){
    int k, m;
    int cc;
    if(  bordvalue[py][px] == 'O' || bordvalue[py][px] == 'X' )
        return 0;

    cc = bordcal(px, py);
    if (cc>0)
        return 1;
    else
        return 0;
}

/* 화면 출력 메소드 */
void main_before_login(){
    WINDOW *main_page_up;
    WINDOW *main_page_under;
    
    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();

    main_page_up = newwin(18, 80, 0, 0);
    main_page_under = newwin(6, 80, 18, 0);

    keypad(main_page_under, TRUE);

    wbkgd(main_page_up , COLOR_PAIR(1));
    wbkgd(main_page_under , COLOR_PAIR(2));
    curs_set(0);

    wprintw(main_page_up, "\n\n\n\n\n\n\n\t\t\t    System Software Practice");
    wprintw(main_page_up, "\n\n \t\t\t\t     OTHELLO");
    wprintw(main_page_up, "\n\n\n\n \t\t\t\t\t\t\t\t 2015726091");
    wprintw(main_page_up, "\n\n \t\t\t\t\t\t\t\t Kwon Sunho");

    wattron(main_page_under, A_REVERSE);
    mvwprintw( main_page_under, 3, 8, before_login[1] );
    wattroff(main_page_under, A_REVERSE);
    mvwprintw(main_page_under, 3, 8+27, before_login[2]);
    mvwprintw(main_page_under, 3, 8+27+27, before_login[3]);

    wmove(main_page_under, 3, 8);
    wrefresh(main_page_up);
    wrefresh(main_page_under);
    
    curenter = rlHandler(main_page_under);
}

void main_after_login(){
    WINDOW *main_page_up;
    WINDOW *main_page_under;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();

    main_page_up = newwin(18, 80, 0, 0);
    main_page_under = newwin(6, 80, 18, 0);

    keypad(main_page_under , TRUE);

    wbkgd(main_page_up , COLOR_PAIR(1));
    wbkgd(main_page_under , COLOR_PAIR(2));
    curs_set(0);
    wprintw(main_page_up, "\n\n\n\n\n\n\n\t\t\t\t Player ID : %s", user.name);

    wattron(main_page_under , A_REVERSE);
    mvwprintw(main_page_under, 3, 9, before_play[1]);
    wattroff(main_page_under, A_REVERSE);
    mvwprintw(main_page_under, 3, 9 + 27, before_play[2]);
    mvwprintw(main_page_under, 3, 9 + 27+27, before_play[3]);

    wmove(main_page_under, 3, 9);

    wrefresh(main_page_up);
    wrefresh(main_page_under);

    curenter = rlHandler(main_page_under);
}

void sign_up(){
    WINDOW *sign_up_page_up;
    WINDOW *sign_up_page_under;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();

    sign_up_page_up = newwin(18, 80, 0, 0);
    sign_up_page_under = newwin(6, 80, 18, 0);

    keypad(sign_up_page_under, TRUE);

    wbkgd(sign_up_page_up , COLOR_PAIR(1));
    wbkgd(sign_up_page_under , COLOR_PAIR(2));

    curs_set(1);

    wprintw(sign_up_page_up, "\n\n\n\n\n\t\t\t\t  SIGN UP");
    wprintw(sign_up_page_up, "\n\n\t\t\t   ID : ");
    wprintw(sign_up_page_up, "\n\n\t\t     PASSWORD : ");

    mvwprintw(sign_up_page_under, 3, 10, sign_up_menu[1]);
    mvwprintw(sign_up_page_under, 3, 10+50, sign_up_menu[2]);

    wmove(sign_up_page_up, 7, 32);

    wrefresh(sign_up_page_under);
    wrefresh(sign_up_page_up);

    nowvalue = inputdata(sign_up_page_up);

    wattron(sign_up_page_under, A_REVERSE);
    mvwprintw(sign_up_page_under, 3, 10, sign_up_menu[1]);
    wattroff(sign_up_page_under, A_REVERSE);
    wmove(sign_up_page_under , 3, 10);

    wrefresh(sign_up_page_under);

    curenter = sign_page_cur_Handler(sign_up_page_under,sign_up_page_up, nowvalue);
}

void sign_in(){
    WINDOW *sign_in_page_up;
    WINDOW *sign_in_page_under;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();

    sign_in_page_up = newwin(18, 80, 0, 0);
    sign_in_page_under = newwin(6, 80, 18, 0);

    keypad(sign_in_page_under, TRUE);

    wbkgd(sign_in_page_up , COLOR_PAIR(1));
    wbkgd(sign_in_page_under , COLOR_PAIR(2));

    curs_set(1);

    wprintw(sign_in_page_up, "\n\n\n\n\n\t\t\t\t  SIGN IN");
    wprintw(sign_in_page_up, "\n\n\t\t\t   ID : ");
    wprintw(sign_in_page_up, "\n\n\t\t     PASSWORD : ");

    mvwprintw(sign_in_page_under, 3, 11, sign_in_menu[1]);
    mvwprintw(sign_in_page_under, 3, 11+50, sign_in_menu[2]);

    wmove(sign_in_page_up, 7, 32);

    wrefresh(sign_in_page_up);
    wrefresh(sign_in_page_under);

    nowvalue = inputdata(sign_in_page_up);

    wattron(sign_in_page_under, A_REVERSE);
    mvwprintw(sign_in_page_under, 3, 11, sign_in_menu[1]);
    wattroff(sign_in_page_under, A_REVERSE);
    wmove(sign_in_page_under , 3, 11);

    wrefresh(sign_in_page_under);

    curenter = sign_in_cur_Handler(sign_in_page_under, sign_in_page_up, nowvalue);
}

void withdrawal(){
    WINDOW *withdrawal_page_up;
    WINDOW *withdrawal_page_under;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();

    withdrawal_page_up = newwin(18, 80, 0, 0);
    withdrawal_page_under = newwin(6, 80, 18, 0);
    keypad(withdrawal_page_under , TRUE);
    wbkgd(withdrawal_page_up , COLOR_PAIR(1));
    wbkgd(withdrawal_page_under , COLOR_PAIR(2));
    curs_set(1);
    wprintw(withdrawal_page_up, "\n\n\n\n\n\t\t\t\t  WITHDRAWAL");
    wprintw(withdrawal_page_up, "\n\n\t\t\t   ID : ");
    wprintw(withdrawal_page_up, "\n\n\t\t     PASSWORD : ");

    mvwprintw(withdrawal_page_under, 3, 7, with_mes[1]);
    mvwprintw(withdrawal_page_under, 3, 7+58, with_mes[2]);

    wmove(withdrawal_page_up, 7, 32);

    wrefresh(withdrawal_page_up);
    wrefresh(withdrawal_page_under);

    nowvalue = inputdata(withdrawal_page_up);

    wattron(withdrawal_page_under, A_REVERSE);
    mvwprintw(withdrawal_page_under, 3, 7, with_mes[1]);
    wattroff(withdrawal_page_under, A_REVERSE);
    wmove(withdrawal_page_under, 3, 7);

    wrefresh(withdrawal_page_under);

    curenter = with_cur_handler(withdrawal_page_under, withdrawal_page_up, nowvalue);
}

void playerInfomation(){
    WINDOW *playinfo_page_up;
    WINDOW *playinfo_page_under;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();
    if ( user.win == 0 && user.lose == 0)
        per = 0;
    else
        per = (user.win / (user.win + user.lose)) * 100;
    playinfo_page_up = newwin(18, 80, 0, 0);
    playinfo_page_under = newwin(6, 80, 18, 0);
    keypad(playinfo_page_under, TRUE);
    wbkgd(playinfo_page_up , COLOR_PAIR(1));
    wbkgd(playinfo_page_under , COLOR_PAIR(2));
    curs_set(0);
    mvwprintw(playinfo_page_under, 3, 35, "WAITING");

    mvwprintw(playinfo_page_up, 5, 11, "PLAYER ID : %s", user.name);
    mvwprintw(playinfo_page_up, 7, 13, "STATISTICS");
    mvwprintw(playinfo_page_up, 9, 6, "WIN : %d ", user.win);
    wprintw(playinfo_page_up, "/ LOSE : %d ", user.lose);
    wprintw(playinfo_page_up, "(%f\%)", per);

    mvwprintw(playinfo_page_up, 5, 51, "PLAYER ID : ");
    mvwprintw(playinfo_page_up, 7, 53, "STATISTICS");
    mvwprintw(playinfo_page_up, 9, 46, "WIN :   ");
    wprintw(playinfo_page_up, "/ LOSE :   ");
    wprintw(playinfo_page_up, "(%f\%)", per);
    curs_set(0);

    wrefresh(playinfo_page_up);
    wrefresh(playinfo_page_under);

    waitingPlayer(playinfo_page_up, playinfo_page_under);
    curenter = before_play_enter(playinfo_page_under);
}

void ingamepage() {
    WINDOW *In_game_page_left;
    WINDOW *In_game_page_right;

    initscr();

    if(has_colors() == FALSE){
        puts("Terminal does not support color!");
        endwin();
        return;
    } else{
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE , COLOR_BLACK);
    }

    refresh();


    In_game_page_right = newwin(24,20,0,60);
    In_game_page_left = newwin(24,60,0,0);

    keypad(In_game_page_left, TRUE);
    keypad(In_game_page_right, TRUE);

    wbkgd(In_game_page_left , COLOR_PAIR(1));
    wbkgd(In_game_page_right , COLOR_PAIR(2));

    curs_set(0);

    mvwaddch(In_game_page_right, 12, 7, 'N'|A_UNDERLINE );
    wprintw(In_game_page_right, "EXT TURN");
    mvwaddch(In_game_page_right, 14, 8, 'R' | A_UNDERLINE);
    wprintw(In_game_page_right, "EGAME");
    mvwprintw(In_game_page_right,16, 8, "E");
    waddch(In_game_page_right, 'X' | A_UNDERLINE);
    wprintw(In_game_page_right, "IT");


    wprintw(In_game_page_left, "\n\n\n\n\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");
    wprintw(In_game_page_left, "\n\t\t|   |   |   |   |   |   |");
    wprintw(In_game_page_left, "\n\t\t+---+---+---+---+---+---+");

    printBrdValue( bordvalue,  In_game_page_left, In_game_page_right);

    wrefresh(In_game_page_left);
    wattron(In_game_page_left, A_REVERSE);
    mvwprintw(In_game_page_left, 10, 26, "%c", bordvalue[2][2]);
    wattroff(In_game_page_left, A_REVERSE);

    wmove(In_game_page_left, 10, 26);

    wrefresh(In_game_page_right);
    wrefresh(In_game_page_left);

    playGame(In_game_page_left, In_game_page_right);
}

User* inputdata(WINDOW * win){
    char userName[MAX + 1] = {'\0',};
    char userPw[MAX + 1] = {'\0', };
    int i =0;
    User * new_user = (User*)malloc(sizeof(User));
    memset(new_user->name, '\0', MAX+1);
    memset(new_user->pw, '\0', MAX+1);

    echo();
    wmove(win, 7, 32);
    wrefresh(win);
    wgetstr(win, userName);
    strcpy(new_user->name, userName);
    wmove(win , 9, 32);
    noecho();
    wgetstr(win, userPw);
    strcpy(new_user->pw, userPw);
    noecho();

    return new_user;
}

void waitingPlayer(WINDOW* win, WINDOW * win2){
    double eneper = 0;
    user.curenter = 9;
    send(sockfd, &user, sizeof(User), 0);

    recv(sockfd, &enemy, sizeof(User), 0);
    if( enemy.stone == 'O'){
        user.stone = 'X';
    }
    else
        user.stone = 'O';
    mvwprintw(win, 5, 63 , "%s", enemy.name);
    mvwprintw(win, 9, 52, "%d", enemy.win);
    wprintw(win, "/ LOSE : %d ", enemy.lose );
    if( enemy.win == 0 && enemy.lose == 0)
        eneper = 0;
    else   
        eneper = (enemy.win / (enemy.win + enemy.lose))*100;
    wprintw(win, "(%f\%)", eneper);
    wrefresh(win);

    mvwprintw(win2, 3, 35, "\t\t\t\t\t");
    wattron(win2 , A_REVERSE);
    mvwprintw(win2, 3, 40, "OK");
    wattroff(win2, A_REVERSE);
    wmove(win2, 3, 42);
    wrefresh(win2);
}

void playGame(WINDOW* win, WINDOW* win2){
    int ingamecnt = 0;
    while(1){
        printBrdValue(bordvalue,  win, win2);
        wrefresh(win);
        wrefresh(win2);
        recv(sockfd, &user, sizeof(User), 0);
        recv(sockfd, &enemy, sizeof(User), 0);
        if ( user.turncnt == 1){
            ingamecnt = ingamehandler(win, win2);
        }
        else{
            printBrdValue( bordvalue,  win, win2);
            recv(sockfd, &bordvalue ,sizeof(char[6][6]), 0);
        }
        if ( ingamecnt == 10)
            break;
    }
}

int bordcal( int x, int y){
    int i, j , k , m;
    int cnt = 0;
    if( user.stone == 0){
        for( i = x+1; i < 6; i++){  // x~6
            if( bordvalue[y][i] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[y][i] == 'O'){
                    i = 7;
                }
            else{
                for( k = x+1; k < i; k++){
                    cnt--;
                }
                i = 7;
            }
        }
        for( i = x-1; i >= 0; i--){ // 0~x
            if( bordvalue[y][i] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[y][i] == 'O'){
                    i = -1;
                }
            else{
                for( k = x-1; k > i; k--){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y+1 ; i < 6; i++){ // y~6
           if( bordvalue[i][x] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[i][x] == 'O'){
                    i = 7;
                }
            else{
                for( k = y+1; k < i; k++){
                    cnt--;
                }
                i=7;
            }

        }
        for( i = y-1; i >= 0; i--){ // 0~y
            if( bordvalue[i][x] == 'X' )
                cnt++;
            else if ( bordvalue[i][x] == 'O'){
                    i = -1;
                }
            else{
                for( k = y-1; k > i; k--){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x-1; i >= 0 && j>= 0 ; i--, j-- ){  // 대각선 좌상
            if( bordvalue[i][j] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = -1;
                }
            else{
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x+1; i >= 0 && j < 6 ; i--, j++ ){ //대각선 우상
            if( bordvalue[i][j] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = -1;
                }
            else{
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y+1 , j = x-1; i < 6 && j>= 0 ; i++, j-- ){  // 대각선 좌하
            if( bordvalue[i][j] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = 7;
                }
            else{
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    cnt--;
                }
                i = 7;
            }
        }
        for( i = y+1 , j = x+1; i < 6 && j < 6 ; i++, j++ ){   // 대각선 우하
            if( bordvalue[i][j] == 'X' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = 7;
                }
            else{
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    cnt--;
                }
                i = 7;
            }
        }
    }
    else{
        for( i = x+1; i < 6; i++){  // x~6
            if( bordvalue[y][i] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[y][i] == 'X'){
                    i = 7;
                }
            else{
                for( k = x+1; k < i; k++){
                    cnt--;
                }
                i = 7;
            }
        }
        for( i = x-1; i >= 0; i--){ // 0~x
            if( bordvalue[y][i] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[y][i] == 'X'){
                    i = -1;
                }
            else{
                for( k = x-1; k > i; k--){
                   cnt--;
                }
                i = -1;
            }
        }
        for( i = y+1 ; i < 6; i++){ // y~6
           if( bordvalue[i][x] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][x] == 'X'){
                    i = 7;
                }
            else{
                for( k = y+1; k < i; k++){
                    cnt--;
                }
                i=7;
            }

        }
        for( i = y-1; i >= 0; i--){ // 0~y
            if( bordvalue[i][x] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][x] == 'X'){
                    i = -1;
                }
            else{
                for( k = y-1; k > i; k--){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x-1; i >= 0 && j>= 0 ; i--, j-- ){  // 대각선 좌상
            if( bordvalue[i][j] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = -1;
                }
            else{
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x+1; i >= 0 && j < 6 ; i--, j++ ){ //대각선 우상
            if( bordvalue[i][j] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = -1;
                }
            else{
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    cnt--;
                }
                i = -1;
            }
        }
        for( i = y+1 , j = x-1; i < 6 && j>= 0 ; i++, j-- ){  // 대각선 좌하
            if( bordvalue[i][j] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = 7;
                }
            else{
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    cnt--;
                }
                i = 7;
            }
        }
        for( i = y+1 , j = x+1; i < 6 && j < 6 ; i++, j++ ){   // 대각선 우하
            if( bordvalue[i][j] == 'O' ){
                cnt++;
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = 7;
                }
            else{
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    cnt--;
                }
                i = 7;
            }
        }

    }
    if ( cnt >= 0)
        return 1;
    return 0;
}