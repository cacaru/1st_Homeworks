/*
 * Student ID : 2015726091
 * Name  : Kwon Sunho
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define MAX_LQ_SIZE 5
#define MAX_BUF_SIZE 1024
#define PERMS 0666
#define MAX 20

struct __User{
    char name[MAX + 1];
    char pw[MAX + 1];
    int curenter;
    int win;
    int lose;
    int stone;
    int stoneNum;
    int turncnt;
};
typedef struct __User User;

void errorHandler(const char *msg);
void signalHandler(int signum);
void cursurhandler(int cursur, int);
int getMaxFd(void);
int rmClient(int n);
int addClient(void);
void playGame(void);
void bordcal( int[2], int);
void stcnt();

int idpw_read(int fd, char *userName);
int idpw_write(User nowuser);
int idpw_read_check(int fd, char *userName, char *userPw);
int idpw_write_check(User user , int i);
int idpw_write_with(User nowuser);
int idpw_read_with(int fd, char *userName, char * userPw);


int sockfd = 0;
int clntNum = 0;
int clntSock[2] = {-1, };
User user[2];
User nowUser[2];
int id;
int point[2];
int ready[2];
int ost;
int xst;
char bordvalue[6][6] = { {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', 'O', 'X', ' ', ' ' },
                         {' ', ' ', 'X', 'O', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' }
                        };

char initbordvalue[6][6] = { {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', 'O', 'X', ' ', ' ' },
                         {' ', ' ', 'X', 'O', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' },
                         {' ', ' ', ' ', ' ', ' ', ' ' }
                        };

int main(int argc, char const *argv[]) {
    fd_set readfds;
    struct sockaddr_in servAddr = { 0x00, };  // 서버의 주소 정보
    int cursur = 0;

    signal(SIGINT, signalHandler);
    signal(SIGPIPE, signalHandler);

    if (argc < 2) {
        printf("Usage: [port]\n");
        return -1;
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) { 
        errorHandler("sock() error!"); 
    }

    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
        errorHandler("bind() error!");
    }

    if (listen(sockfd, MAX_LQ_SIZE) == -1){
        errorHandler("listen()error!");
    }

    puts("[start othello server]");

    while(1){   
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        for(int i = 0; i< clntNum; i++) { FD_SET(clntSock[i], &readfds);  }
        
        if (select(getMaxFd(), &readfds, NULL, NULL, NULL) < 0) {
            errorHandler("select() error!");
        }

        if (FD_ISSET(sockfd, &readfds)) {
            if (addClient() == -1) { puts("addClient() error!"); }
        }
        for(int i = 0; i < clntNum; i++) {
            if (FD_ISSET(clntSock[i], &readfds)) {
                recv(clntSock[i], &user[i], sizeof(User), 0);
                printf("Username : %s , pw : %s \n", user[i].name, user[i].pw);
                cursurhandler(user[i].curenter, i);
            }
        }
 
    }
    puts("Server END");
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

void errorHandler(const char *msg) {
    for(int i = 0; i< clntNum; ++i) { close(clntSock[i]); }
    close(sockfd);
    fputs(msg, stderr);
    exit(-1);
}

void cursurhandler(int cursur, int i){
    printf("curenter %d\n", cursur);
    switch(cursur){
        case 7:    // 회원탈퇴
            id = idpw_write_with(user[i]);
            send(clntSock[i], &id , sizeof(int), 0);
            memset(nowUser[i].name, '\0', MAX+1);
            memset(nowUser[i].pw, '\0', MAX+1);
            break;
        case 9:     //게임 대기방.
            if( clntNum == 2 && user[0].curenter == user[1].curenter ){
                nowUser[0].stone = 1; nowUser[1].stone = 2;
                send(clntSock[0], &nowUser[1], sizeof(User), 0);
                send(clntSock[1], &nowUser[0], sizeof(User), 0);
            }
            break;
        case 10:    //회원 가입
            id = idpw_write(user[i]);
            user[i].win = 0; user[i].lose = 0; user[i].curenter = id;
            send(clntSock[i], &user[i], sizeof(User), 0);
            id = 0;
            break;
        case 11:    //로그인
            id = idpw_write_check(user[i], i);
            user[i].curenter = id;
            send(clntSock[i], &user[i], sizeof(User), 0);
            id = 0;
            break;
        case 36:    //로그아웃
            printf(" >>> %s logout \n", nowUser[i].name);
            memset(nowUser[i].name , '\0', MAX+1);
            memset(nowUser[i].pw , '\0', MAX + 1);
            break;
        case 42:        //인게임 입장.
            if ( user[0].curenter == user[1].curenter ){
                printf("Game Start : %s, %s\n", nowUser[0].name, nowUser[1].name);
                printf("Game : %s  %d %d\n", nowUser[0].name, nowUser[0].win, nowUser[0].lose);
                printf("Game : %s  %d %d\n", nowUser[1].name, nowUser[1].win, nowUser[1].lose);
                send(clntSock[0], &bordvalue, sizeof(char[6][6]), 0);
                send(clntSock[1], &bordvalue, sizeof(char[6][6]), 0);
                ost = 2; xst = 2;
                playGame();
            }
            break;
        case 62 :       // 게임 종료
            printf(">> connect out client %d \n", i+1);
            rmClient(i);
            break;
        default:        //그외 에러
            printf("Error value from client %d \n", i+1);
            break;
    }
}

int getMaxFd(void) {
    int i = 0;
    int max = sockfd;

    for(i = 0; i < clntNum; ++i) {
        if (max < clntSock[i]) { max = clntSock[i]; }
    }

    return max + 1;
}

int addClient(void) {
    int clnt = 0;
    int clntAddrSize = 0;
    struct sockaddr_in clntAddr = { 0x00, };

    if ((clnt = accept(sockfd, (struct sockaddr *)&clntAddr, &clntAddrSize)) == -1) {
        errorHandler("accept() error!");
    }

    if (clntNum == 2) { return -1; }

    clntSock[clntNum] = clnt;
    ++clntNum;

    printf("Connect : %d\n", clntNum);

    return 0;
}

int rmClient(int n) {
    close(clntSock[n]);

    if (n != clntNum - 1) {
        clntSock[n] = clntSock[clntNum - 1];
        clntSock[clntNum - 1] = -1;
    }
    --clntNum;
    if( clntNum == 0){
        memset(nowUser[0].name , '\0', MAX + 1);
        memset(nowUser[0].pw , '\0', MAX+1);
    }
}

void playGame(){
    nowUser[0].turncnt = 1;  nowUser[1].turncnt = 0;
    int nowTurnPlayer = 0;
    while(1){
        stcnt();
        nowUser[0].stoneNum = ost; nowUser[1].stoneNum = xst;
        send(clntSock[0], &nowUser[0], sizeof(User), 0 );
        send(clntSock[0], &nowUser[1], sizeof(User), 0 );
        send(clntSock[1], &nowUser[1], sizeof(User), 0 );
        send(clntSock[1], &nowUser[0], sizeof(User), 0 );

        recv(clntSock[nowTurnPlayer], &point, sizeof(int[2]), 0);

        bordcal( point , nowTurnPlayer);
        printf("%d  %d \n", ost, xst);
        send(clntSock[0], &bordvalue, sizeof(char[6][6]), 0 );
        send(clntSock[1], &bordvalue, sizeof(char[6][6]), 0 );

        if( nowUser[0].turncnt == 1){
            nowUser[0].turncnt = 0; nowUser[1].turncnt = 1;
            nowTurnPlayer = 1;
        }
        else{
            nowUser[0].turncnt = 1; nowUser[1].turncnt = 0;
            nowTurnPlayer = 0;
        }
        if( (nowUser[0].stoneNum + nowUser[1].stoneNum) == 36){
            break;
        }
    }
}

void bordcal( int po[2] , int player){
    int x = po[1]; 
    int y = po[0];
    int i, j , k , m;

    if( player == 0){
        bordvalue[y][x] = 'O';
        for( i = x+1; i < 6; i++){  // x~6
            if( bordvalue[y][i] == 'X' ){
                bordvalue[y][i] = 'O';
            }
            else if ( bordvalue[y][i] == 'O'){
                    i = 7;
                }
            else if ( i == 5 ){
                for( k = x+1; k < i; k++){
                    bordvalue[y][k] = 'X';
                }
                i = 7;
            }
            else{
                for( k = x+1; k < i; k++){
                    bordvalue[y][k] = 'X';
                }
                i = 7;
            }
        }
        for( i = x-1; i >= 0; i--){ // 0~x
            if( bordvalue[y][i] == 'X' ){
                bordvalue[y][i] = 'O';
            }
            else if ( bordvalue[y][i] == 'O'){
                    i = -1;
                }
            else if ( i == 0 ){
                for( k = x-1; k > i; k--){
                    bordvalue[y][k] = 'X';
                }
                i = -1;
            }
            else{
                for( k = x-1; k > i; k--){
                    bordvalue[y][k] = 'X';
                }
                i = -1;
            }
        }
        for( i = y+1 ; i < 6; i++){ // y~6
           if( bordvalue[i][x] == 'X' ){
               bordvalue[i][x] = 'O'; 
            }
            else if ( bordvalue[i][x] == 'O'){
                    i = 7;
                }
            else if ( i == 5){
                for( k = y+1; k < i; k++){
                    bordvalue[k][x] = 'X';
                }
                i=7;
            }
            else{
                for( k = y+1; k < i; k++){
                    bordvalue[k][x] = 'X';
                }
                i=7;
            }

        }
        for( i = y-1; i >= 0; i--){ // 0~y
            if( bordvalue[i][x] == 'X' ){
                bordvalue[i][x] = 'O'; 
            }
            else if ( bordvalue[i][x] == 'O'){
                    i = -1;
                }
            else if ( i == 0){
                for( k = y-1; k > i; k--){
                    bordvalue[k][x] = 'X';
                }
                i = -1;
            }
            else{
                for( k = y-1; k > i; k--){
                    bordvalue[k][x] = 'X';
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x-1; i >= 0 && j>= 0 ; i--, j-- ){  // 대각선 좌상
            if( bordvalue[i][j] == 'X' ){
                bordvalue[i][j] = 'O';
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = -1;
                }
            else if ( i == 0 || j == 0){
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    bordvalue[k][m] = 'X';
                }
                i = -1;
            }
            else{
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    bordvalue[k][m] = 'X';
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x+1; i >= 0 && j < 6 ; i--, j++ ){ //대각선 우상
            if( bordvalue[i][j] == 'X' ){
                bordvalue[i][j] = 'O';
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = -1;
                }
            else if (i == 0 || j == 5){
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    bordvalue[k][m] = 'X';
                }
                i = -1;
            }
            else{
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    bordvalue[k][m] = 'X';
                }
                i = -1;
            }
        }
        for( i = y+1 , j = x-1; i < 6 && j>= 0 ; i++, j-- ){  // 대각선 좌하
            if( bordvalue[i][j] == 'X' ){
                bordvalue[i][j] = 'O';
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = 7;
                }
            else if ( i == 5 || j == 0){
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    bordvalue[k][m] = 'X';
                }
                i = 7;
            }
            else{
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    bordvalue[k][m] = 'X';
                }
                i = 7;
            }
        }
        for( i = y+1 , j = x+1; i < 6 && j < 6 ; i++, j++ ){   // 대각선 우하
            if( bordvalue[i][j] == 'X' ){
                bordvalue[i][j] = 'O';
            }
            else if ( bordvalue[i][j] == 'O'){
                    i = 7;
                }
            else if ( i == 5 || j == 5){
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    bordvalue[k][m] = 'X';
                }
                i = 7;
            }
            else{
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    bordvalue[k][m] = 'X';
                }
                i = 7;
            }
        }
    }
    else{
        bordvalue[y][x] = 'X';
        for( i = x+1; i < 6; i++){  // x~6
            if( bordvalue[y][i] == 'O' ){
                bordvalue[y][i] = 'X';
            }
            else if ( bordvalue[y][i] == 'X'){
                    i = 7;
                }
            else if ( i == 5 ){
                for( k = x+1; k < i; k++){
                    bordvalue[y][k] = 'O';
                }
                i = 7;
            }
            else{
                for( k = x+1; k < i; k++){
                    bordvalue[y][k] = 'O';
                }
                i = 7;
            }
        }
        for( i = x-1; i >= 0; i--){ // 0~x
            if( bordvalue[y][i] == 'O' ){
                bordvalue[y][i] = 'X';
            }
            else if ( bordvalue[y][i] == 'X'){
                    i = -1;
                }
            else if ( i == 0 ){
                for( k = x-1; k > i; k--){
                    bordvalue[y][k] = 'O';
                }
                i = -1;
            }
            else{
                for( k = x-1; k > i; k--){
                    bordvalue[y][k] = 'O';
                }
                i = -1;
            }
        }
        for( i = y+1 ; i < 6; i++){ // y~6
           if( bordvalue[i][x] == 'O' ){
               bordvalue[i][x] = 'X'; 
            }
            else if ( bordvalue[i][x] == 'X'){
                    i = 7;
                }
            else if ( i == 5){
                for( k = y+1; k < i; k++){
                    bordvalue[k][x] = 'O';
                }
                i=7;
            }
            else{
                for( k = y+1; k < i; k++){
                    bordvalue[k][x] = 'O';
                }
                i=7;
            }

        }
        for( i = y-1; i >= 0; i--){ // 0~y
            if( bordvalue[i][x] == 'O' ){
                bordvalue[i][x] = 'X'; 
            }
            else if ( bordvalue[i][x] == 'X'){
                    i = -1;
                }
            else if ( i == 0){
                for( k = y-1; k > i; k--){
                    bordvalue[k][x] = 'O';
                }
                i = -1;
            }
            else{
                for( k = y-1; k > i; k--){
                    bordvalue[k][x] = 'O';
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x-1; i >= 0 && j>= 0 ; i--, j-- ){  // 대각선 좌상
            if( bordvalue[i][j] == 'O' ){
                bordvalue[i][j] = 'X';
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = -1;
                }
            else if ( i == 0 || j == 0){
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    bordvalue[k][m] = 'O';
                }
                i = -1;
            }
            else{
                for( k = y-1 , m = x-1; k > i && m > j; k-- , m--){
                    bordvalue[k][m] = 'X';
                }
                i = -1;
            }
        }
        for( i = y-1 , j = x+1; i >= 0 && j < 6 ; i--, j++ ){ //대각선 우상
            if( bordvalue[i][j] == 'O' ){
                bordvalue[i][j] = 'X';
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = -1;
                }
            else if (i == 0 || j == 5){
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    bordvalue[k][m] = 'O';
                }
                i = -1;
            }
            else{
                for( k = y-1 , m = x+1; k > i && m < j; k-- , m++){
                    bordvalue[k][m] = 'O';
                }
                i = -1;
            }
        }
        for( i = y+1 , j = x-1; i < 6 && j>= 0 ; i++, j-- ){  // 대각선 좌하
            if( bordvalue[i][j] == 'O' ){
                bordvalue[i][j] = 'X';
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = 7;
                }
            else if ( i == 5 || j == 0){
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    bordvalue[k][m] = 'O';
                }
                i = 7;
            }
            else{
                for( k = y+1 , m = x-1; k < i && m > j; k++ , m--){
                    bordvalue[k][m] = 'O';
                }
                i = 7;
            }
        }
        for( i = y+1 , j = x+1; i < 6 && j < 6 ; i++, j++ ){   // 대각선 우하
            if( bordvalue[i][j] == 'O' ){
                bordvalue[i][j] = 'X';
            }
            else if ( bordvalue[i][j] == 'X'){
                    i = 7;
                }
            else if ( i == 5 || j == 5){
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    bordvalue[k][m] = 'O';
                }
                i = 7;
            }
            else{
                for( k = y+1 , m = x+1; k < i && m < j; k++ , m++){
                    bordvalue[k][m] = 'O';
                }
                i = 7;
            }
        }
    }

}

void stcnt(){
    ost = 0; xst = 0;
    int w, b;
    for( w = 0; w < 6 ; w++){
        for( b = 0; b< 6 ; b++){
            if( bordvalue[w][b] == 'O')
                ost++;
            else if ( bordvalue[w][b] == 'X')
                xst++;
        }
    }
}

//파일 입출력
int idpw_write(User nowuser){
    int fd = 0, uc = 1;
    char * pathname = "./2015726091.txt";
    ssize_t wsize = 0;
    
    char userName[MAX + 1] = {'\0',};
    char userPw[MAX + 1] = {'\0', };
    
    fd = open(pathname, O_CREAT | O_APPEND | O_RDWR , PERMS);
    if( fd == -1){
        perror("open error");
        exit(-1);
    }

    uc = idpw_read(fd, nowuser.name);
    if( uc > 0){
        return 2;
    }
    else if ( uc == 0){
        wsize = write(fd, (User*)&nowuser, sizeof(User));
        if( wsize == -1){
            perror("write error");
            exit(-2);
        }
    }

    close(fd);
    return 1;
}

int idpw_read(int fd, char *userName){
    ssize_t rsize =0;
    lseek(fd, (off_t)0, SEEK_SET);
    User * be_user = (User*)malloc(sizeof(User));
    memset(be_user->name, '\0', MAX+1);
    memset(be_user->pw, '\0', MAX+1);

    do {
        memset(be_user->name, '\0', MAX+1);
        memset(be_user->pw, '\0', MAX+1);

        rsize = read(fd, (User*)be_user, sizeof(User));
        if( rsize == -1){
            perror("read error");
            exit(-2);
        }

        if ( !strcmp(be_user->name,userName) ){
            rsize = -1;
        }
        
    }while( rsize > 0);

    if (!strcmp(be_user->name, userName))
        return 1;

    free(be_user);
    return 0;
}

int idpw_write_check( User user, int i){
    int fd = 0, uc = 1;
    char * pathname = "./2015726091.txt";
    ssize_t wsize = 0;
    
    char userName[MAX + 1] = {'\0',};
    char userPw[MAX + 1] = {'\0', };
    
    fd = open(pathname, O_CREAT | O_APPEND | O_RDWR , PERMS);
    if( fd == -1){
        perror("open error");
        exit(-1);
    }
    uc = idpw_read_check(fd, user.name, user.pw);

    if (uc == 2){
        return 2;
    }
    else if (uc == 1){
        for( int j =0; j< clntNum; j++){
            if( !strcmp(user.name , nowUser[j].name) ){
                return 3;
            }
        }
        strcpy(nowUser[i].name , user.name);
        strcpy(nowUser[i].pw , user.pw);

        close(fd);
        return 1;
    }
    else{
        perror("file check error");
        exit(-3);
    }
}

int idpw_read_check(int fd, char *userName, char *userPw){
    ssize_t rsize =0;
    lseek(fd, (off_t)0, SEEK_SET);
    User * be_user = (User*)malloc(sizeof(User));
    memset(be_user->name, '\0', MAX+1);
    memset(be_user->pw, '\0', MAX+1);

    do {
        memset(be_user->name, '\0', MAX+1);
        memset(be_user->pw, '\0', MAX+1);

        rsize = read(fd, (User*)be_user, sizeof(User));
        if( rsize == -1){
            perror("read error");
            exit(-2);
        }

        if ( !strcmp(be_user->name, userName)){
            if( !strcmp(be_user->pw ,userPw)){
                free(be_user);
                return 1;
            }
        }
        
    }while( rsize > 0);

    free(be_user);
    return 2;
}

int idpw_write_with(User nowuser){
    int fd = 0, uc = 1;
    char * pathname = "./2015726091.txt";
    ssize_t wsize = 0;
    
    char userName[MAX + 1] = {'\0',};
    char userPw[MAX + 1] = {'\0', };

    fd = open(pathname, O_RDWR);
    if( fd == -1){
        perror("open error");
        exit(-1);
    }

    uc = idpw_read_with(fd, nowuser.name, nowuser.pw);

    if( uc == 2){
        return 2;
    }
    else{
        close(fd);
        return 1;
    }
}

int idpw_read_with(int fd, char *userName, char * userPw){
    ssize_t rsize =0;
    ssize_t wsize = 0;
    char empty_name[20];
    char empty_pw[20];
    memset(empty_name, '\0', MAX);
    memset(empty_pw, '\0', MAX);
    lseek(fd, (off_t)0, SEEK_SET);
    User * be_user = (User*)malloc(sizeof(User));
    memset(be_user->name, '\0', MAX+1);
    memset(be_user->pw, '\0', MAX+1);

    do {
        memset(be_user->name, '\0', MAX+1);
        memset(be_user->pw, '\0', MAX+1);

        rsize = read(fd, (User*)be_user, sizeof(User));
        if( rsize == -1){
            perror("read error");
            exit(-2);
        }

        if ( !strcmp(be_user->name, userName)){
            if( !strcmp(be_user->pw ,userPw)){
                lseek(fd, (off_t)-sizeof(User), SEEK_CUR);
                break;
            }
        }
        
    }while( rsize > 0);

    if( rsize > 0){
        strcpy(be_user->name, empty_name);
        strcpy(be_user->pw , empty_pw);
        wsize = write(fd, (User*)be_user, sizeof(User));
        if(wsize == -1){
            perror("write error");
            exit(-2);
        }

        free(be_user);
        return 1;
    }

    free(be_user);
    return 2;
}
