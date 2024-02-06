#include <stdlib.h>

#include <stdio.h>
#include <errno.h>  

#include <string.h>
//#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

char* cwd = NULL;

void showDir(char* pathname){
    struct dirent **namelist;
    int count = scandir((const char*)(pathname), &namelist, NULL, alphasort);
    if (count < 0) {
        perror("scandir error");
        exit(EXIT_FAILURE);
    }
    //printf("Total files: %d\n", count);

    for (int i = 0; i < count; i++) {
        if (namelist[i]->d_type == DT_DIR){
            printf("%s/\n", namelist[i]->d_name);
            if (i == 2){
                strcat(cwd, "/");
                strcat(cwd, namelist[i]->d_name);
                printf("====== IN DIR: %s\n", cwd);
                int cwd_len = strlen(cwd);
                int name_len = strlen(namelist[i]->d_name);
                cwd[cwd_len-1-name_len] = '\0';
            }
        }else{
            printf("%s\n", namelist[i]->d_name);
        }

        free(namelist[i]);
    }

    free(namelist);
}

int setDir(char* pathname, WINDOW* window){
    struct dirent **namelist;
    int count = scandir((const char*)(pathname), &namelist, NULL, alphasort);
    if (count < 0) {
        perror("scandir error");
        exit(EXIT_FAILURE);
    }
    //printf("Total files: %d\n", count);

    for (int i = 0; i < count; i++) {
        if (namelist[i]->d_type == DT_DIR){
            printf("%s/\n", namelist[i]->d_name);
            if (i == 2){
                strcat(cwd, "/");
                strcat(cwd, namelist[i]->d_name);
                printf("====== IN DIR: %s\n", cwd);
                int cwd_len = strlen(cwd);
                int name_len = strlen(namelist[i]->d_name);
                cwd[cwd_len-1-name_len] = '\0';
            }
        }else{
            printf("%s\n", namelist[i]->d_name);
        }

        free(namelist[i]);
    }

    free(namelist);
}

int main(int argc, char ** argv){

    cwd = calloc(PATH_MAX, 1);
    char _cwd[PATH_MAX];
    char* _cwd_ptr = getcwd(_cwd, sizeof(_cwd));
    if (_cwd_ptr != NULL) {
        //printf("getcwd directory: %s\n", _cwd);
    } else {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
    strcpy(cwd, _cwd);


    WINDOW * wnd;
    WINDOW * subwnd;
    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    const int wnd_h = 50;
    const int wnd_w = 120;

    wnd = newwin(wnd_h, wnd_w, 0, 0);
    box(wnd, '|', '-');
    wrefresh(wnd);

    subwnd = derwin(wnd, wnd_h-2, wnd_w-2, 1, 1);
    //box(subwnd, '|', '-');
    //wrefresh(wnd);

    mvwprintw(subwnd, 1, 1, "Hello, brave new curses world!");
    mvwprintw(subwnd, 2, 1, "Hello, brave new curses world2!");
    wrefresh(subwnd);

    int ch;
    while ((ch = getch()) != 'q') {
        switch(ch) {
            case KEY_UP: // Page Up
                mvwprintw(wnd, 4, 1, "Page Up");
                wrefresh(wnd);
                break;
            case KEY_DOWN: // Page Down
                mvwprintw(wnd, 5, 1, "Page Down");
                wrefresh(wnd);
                break;
            case '\n': // Enter
                mvwprintw(wnd, 6, 1, "Enter");
                wrefresh(wnd);
                break;
            default:
                break;
        }
    }

    //delwin(subwnd);
    delwin(wnd);

    //getch();
    endwin();
    exit(EXIT_SUCCESS);


    free(cwd);
    return 0;
}





// void showDir(char* pathname){
//     struct dirent **namelist;
//     int count = scandir((const char*)(pathname), &namelist, NULL, alphasort);
//     if (count < 0) {
//         perror("scandir error");
//         exit(EXIT_FAILURE);
//     }
//     //printf("Total files: %d\n", count);

//     for (int i = 0; i < count; i++) {
//         if (namelist[i]->d_type == DT_DIR){
//             printf("%s/\n", namelist[i]->d_name);
//             if (i == 2){
//                 strcat(cwd, "/");
//                 strcat(cwd, namelist[i]->d_name);
//                 printf("====== IN DIR: %s\n", cwd);
//                 int cwd_len = strlen(cwd);
//                 int name_len = strlen(namelist[i]->d_name);
//                 cwd[cwd_len-1-name_len] = '\0';
//             }
//         }else{
//             printf("%s\n", namelist[i]->d_name);
//         }

//         free(namelist[i]);
//     }

//     free(namelist);
// }

// int main(int argc, char ** argv){

//     cwd = calloc(PATH_MAX, 1);
//     char _cwd[PATH_MAX];
//     char* _cwd_ptr = getcwd(_cwd, sizeof(_cwd));
//     if (_cwd_ptr != NULL) {
//         //printf("getcwd directory: %s\n", _cwd);
//     } else {
//         perror("getcwd() error");
//         exit(EXIT_FAILURE);
//     }
//     strcpy(cwd, _cwd);
//     printf("====== PRE DIR: %s\n", cwd);

//     showDir(cwd);
//     printf("====== AFTER DIR: %s\n", cwd);


//     free(cwd);
//     return 0;
// }




/*
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char ** argv)
{

    WINDOW * wnd;
    WINDOW * subwnd;
    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    wnd = newwin(20, 40, 0, 0);
    box(wnd, '|', '-');
    wrefresh(wnd);

    // subwnd = derwin(wnd, 10, 35, 1, 1);
    // box(subwnd, '|', '-');
    // wrefresh(wnd);

    // mvwprintw(subwnd, 1, 1, "Hello, brave new curses world!");
    // mvwprintw(subwnd, 2, 1, "Hello, brave new curses world2!");
    // wrefresh(subwnd);

    int ch;
    while ((ch = getch()) != 'q') {
        switch(ch) {
            case KEY_UP: // Page Up
                mvwprintw(wnd, 4, 1, "Page Up");
                wrefresh(wnd);
                break;
            case KEY_DOWN: // Page Down
                mvwprintw(wnd, 5, 1, "Page Down");
                wrefresh(wnd);
                break;
            case '\n': // Enter
                mvwprintw(wnd, 6, 1, "Enter");
                wrefresh(wnd);
                break;
            default:
                break;
        }
    }

    //delwin(subwnd);
    delwin(wnd);

    //getch();
    endwin();
    exit(EXIT_SUCCESS);



    // WINDOW * wnd;
    // WINDOW * subwnd;
    // initscr();
    // signal(SIGWINCH, sig_winch);
    // cbreak();
    // curs_set(0);
    // refresh();
    // wnd = newwin(6, 18, 2, 4);
    // box(wnd, '|', '-');
    // subwnd = derwin(wnd, 4, 16, 1, 1);
    // wprintw(subwnd, "Hello, brave new curses world!\n");
    // wrefresh(wnd);
    // delwin(subwnd);
    // delwin(wnd);
    // move(9, 0);
    // printw("Press any key to continue...");
    // refresh();
    // getch();
    // endwin();
    // exit(EXIT_SUCCESS);
}
*/