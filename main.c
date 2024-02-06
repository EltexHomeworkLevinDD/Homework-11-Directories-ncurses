#include <stdlib.h>
#include <stdio.h>
#include <errno.h>  
#include <string.h>
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

void writeLine(int line, struct dirent** namelist, WINDOW* window);
void selectLine(int line, struct dirent** namelist, WINDOW* window);
void deSelectLine(int line, struct dirent** namelist, WINDOW* window);
int setDirInfo(char* pathname, struct dirent*** namelist, WINDOW* window);

const int wnd_h = 40;
const int wnd_w = 120;

int main(int argc, char ** argv){

    char cwd[PATH_MAX];
    char* cwd_ptr = getcwd(cwd, sizeof(cwd));
    if (cwd_ptr != NULL) {
        //printf("getcwd directory: %s\n", cwd_ptr);
    } else {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    WINDOW* wnd;
    WINDOW* subwnd;
    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    start_color();
    refresh();
    init_pair(10, COLOR_BLACK, COLOR_CYAN);
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);

    wnd = newwin(wnd_h, wnd_w, 0, 0);
    box(wnd, '|', '-');
    wrefresh(wnd);

    subwnd = derwin(wnd, wnd_h-2, wnd_w-2, 1, 1);

    struct dirent** namelist = NULL;
    int count = setDirInfo(cwd, &namelist, subwnd);

    int ch;
    int line = 1;
    while (ch != 'q') {

        ch = getch();
        switch(ch) {
            case KEY_UP: // Page Up
                if ((line-1) > 0){
                    line--;
                    selectLine(line, namelist, subwnd);
                    deSelectLine(line+1, namelist, subwnd);
                }
                break;
            case KEY_DOWN: // Page Down
                if ( ((line+1) <= count-1) ){
                    line++;
                    selectLine(line, namelist, subwnd);
                    //if ((line-1) != 0)
                        deSelectLine(line-1, namelist, subwnd);
                }
                break;
            case '\n': // Enter
                if (namelist[line]->d_type == DT_DIR){
                    if (line == 1){ // '../'
                        int i = strlen(cwd);
                        while (cwd[i] != '/') {
                            cwd[i] = '\0';
                            i--;
                        }
                        cwd[i] = '\0';
                    }else{
                        strcat(cwd, "/");
                        strcat(cwd, namelist[line]->d_name);
                    }

                    for (int i = 0; i < count; i++)
                        free(namelist[i]);
                    free(namelist);
                    namelist = NULL;
                    line = 1;
                    count = setDirInfo(cwd, &namelist, subwnd);
                }   
                break;
            default:
                break;
        }
    }

    delwin(subwnd);
    delwin(wnd);
    endwin();

    for (int i = 0; i < count; i++)
        free(namelist[i]);
     free(namelist);

    return 0;
}

void writeLine(int line, struct dirent** namelist, WINDOW* window){
    wmove(window, line, 0);  // Переместить курсор в начало строки row
    wclrtoeol(window);      // Очистить строку от текущей позиции курсора до конца строки
    mvwprintw(window, line, 1, "%s", (const char*)(namelist[line]->d_name));
    if (namelist[line]->d_type == DT_DIR){
         wprintw(window, "/");
    }
}

void selectLine(int line, struct dirent** namelist, WINDOW* window){
    wattron(window, COLOR_PAIR(1));
    writeLine(line, namelist, window);
    wattroff(window, COLOR_PAIR(1));
    wrefresh(window);
}

void deSelectLine(int line, struct dirent** namelist, WINDOW* window){
    wattroff(window, COLOR_PAIR(1));
    writeLine(line, namelist, window);
    wrefresh(window);
}


int setDirInfo(char* pathname, struct dirent*** namelist, WINDOW* window){
    int count = scandir((const char*)(pathname), namelist, NULL, alphasort);
    if (count < 0) {
        perror("scandir error");
        exit(EXIT_FAILURE);
    }
    //printf("path: %s\n", pathname);
    wattron(window, COLOR_PAIR(10));
    wmove(window, 0, 0);
    wclrtoeol(window);
    mvwprintw(window, 0, 1, "%s", (const char*)pathname);
    wprintw(window, "/");
    wattroff(window, COLOR_PAIR(10));
    wrefresh(window);   

    selectLine(1, *namelist, window);
    for (int i = 2; i < count; i++) {
        writeLine(i, *namelist, window);
        wrefresh(window);
    }

    for (int i = count; i < wnd_h; i++){
        wmove(window, i, 0);  // Переместить курсор в начало строки row
        wclrtoeol(window);      // Очистить строку от текущей позиции курсора до конца строки
    }
     wrefresh(window);

    return count;
}