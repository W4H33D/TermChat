#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>

#define SER_IP "127.0.0.1"
#define SER_PORT 4444

int sockfd = 0;
WINDOW *header_win, *msg_win, *input_win;
pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;

void update_header(int count) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    pthread_mutex_lock(&ui_mutex);
    // Use the color pair and clear the specific stats line
    wattron(header_win, COLOR_PAIR(1));
    mvwprintw(header_win, 2, 1, "                                                               ");

    // Draw the left side info
    mvwprintw(header_win, 2, 2, "Connected: %s:%d | Status: Active", SER_IP, SER_PORT);

    // Draw the right side info (Online Count)
    mvwprintw(header_win, 2, cols - 20, "Online User: %d", count);

    box(header_win, 0, 0);
    wrefresh(header_win);

    // Ensure the cursor returns to the input box after header update
    wmove(input_win, 1, 3);
    wrefresh(input_win);
    pthread_mutex_unlock(&ui_mutex);
}

void setup_ui() {
    initscr();
    start_color();
    cbreak(); noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    header_win = newwin(4, cols, 0, 0);
    wbkgd(header_win, COLOR_PAIR(1));
    box(header_win, 0, 0);
    mvwprintw(header_win, 1, (cols / 2) - 7, "termChat Client");
    wrefresh(header_win);

    update_header(0);

    msg_win = newwin(rows - 7, cols - 2, 4, 1);
    scrollok(msg_win, TRUE);
    box(msg_win, 0, 0);
    wrefresh(msg_win);

    input_win = newwin(3, cols, rows - 3, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, " ");
    wrefresh(input_win);
}

void* recv_handler(void* arg) {
    char message[2048];
    while (1) {
        int receive = recv(sockfd, message, sizeof(message) - 1, 0);
        if (receive > 0) {
            message[receive] = '\0';

            if (strncmp(message, "", 7) == 0) {
                int count = atoi(message + 7);
                update_header(count);
            } else {
                pthread_mutex_lock(&ui_mutex);
                wprintw(msg_win, " %s", message);
                box(msg_win, 0, 0); // Restore border
                wnoutrefresh(msg_win);
                wnoutrefresh(input_win);
                doupdate();
                pthread_mutex_unlock(&ui_mutex);
            }
        } else if (receive <= 0) break;
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    char name[32];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SER_IP);
    server_addr.sin_port = htons(SER_PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed"); return 1;
    }

    setup_ui();

    // Handshake
    pthread_mutex_lock(&ui_mutex);
    mvwprintw(input_win, 1, 2, "Enter Username: ");
    wrefresh(input_win);
    pthread_mutex_unlock(&ui_mutex);

    echo();
    wgetnstr(input_win, name, 31);
    noecho();
    strtok(name, "\r\n");

    pthread_t tid;
    pthread_create(&tid, NULL, recv_handler, NULL);
    pthread_detach(tid);

    send(sockfd, name, strlen(name), 0);

    pthread_mutex_lock(&ui_mutex);
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, " ");
    wrefresh(input_win);
    pthread_mutex_unlock(&ui_mutex);

    char buffer[1024];
    while (1) {
        wmove(input_win, 1, 3);
        wrefresh(input_win);
        echo();
        int r, c;
        getmaxyx(input_win, r, c);
        wgetnstr(input_win, buffer, c - 5);
        noecho();

        if (strcmp(buffer, "/exit") == 0) break;
        if (strlen(buffer) > 0) send(sockfd, buffer, strlen(buffer), 0);

        pthread_mutex_lock(&ui_mutex);
        wmove(input_win, 1, 3);
        wclrtoeol(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, " ");
        wrefresh(input_win);
        pthread_mutex_unlock(&ui_mutex);
    }

    endwin();
    close(sockfd);
    return 0;
}