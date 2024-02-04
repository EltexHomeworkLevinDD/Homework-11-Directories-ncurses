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
    cbreak(); // функция, считавшая символ, передает его программе, не дожидаясь, пока пользователь нажмет [Enter]
    curs_set(0); // курсор невидим
    refresh(); // необходимо
    wnd = newwin(6, 18, 2, 4); // окно
    box(wnd, '|', '-'); // рамка
    subwnd = derwin(wnd, 4, 16, 1, 1); // подокно
    wprintw(subwnd, "Hello, brave new curses world!\n");
    wrefresh(wnd);
    delwin(subwnd);
    delwin(wnd);
    move(9, 0);
    printw("Press any key to continue...");
    refresh();
    getch();
    endwin();
    exit(EXIT_SUCCESS);
}