#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); // Получить необходимые для resizeterm() значения параметров окна
    resizeterm(size.ws_row, size.ws_col); // Изменить размер окна
}

int main(int argc, char ** argv){
    // Работаем с окном stdcr
    initscr(); // инициализирует структуры дынных ncurses и переводит терминал в нужный режим
    signal(SIGWINCH, sig_winch); // устанавливаем обработчик сигнал SIGWINCH (только после инициализации ncurses)
    cbreak();
    noecho(); // отключает отображение символов, вводимых с клавиатуры
    curs_set(0); // управляет видимостью курсора (невидим)
    attron(A_BOLD); // указать некоторые дополнительные атрибуты символов
    move(5, 15); // устанавливает позицию курсора в окне stdscr
    printw("Hello, brave new curses world!\n");
    attroff(A_BOLD); // Сбросить атрибуты символов можно с помощью функции

    attron(A_BLINK); // Моргающий текст
    move(7, 16);
    printw("Press any key to continue...");
    refresh(); // Для того чтобы напечатанные нами символы стали видимыми
    getch();
    endwin(); // По окончании работы с ncurses
    exit(EXIT_SUCCESS);
}