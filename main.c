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

// Обработчик сигнала SIGWINCH
void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

// Записать строку (после нужно сделать refresh)
void writeLine(int line, struct dirent** namelist, WINDOW* window);
// Выделить строку желтым
void selectLine(int line, struct dirent** namelist, WINDOW* window);
// Удалить выделение желтым
void deSelectLine(int line, struct dirent** namelist, WINDOW* window);
// Скан директории и вывод данных на экран
int setDirInfo(char* pathname, struct dirent*** namelist, WINDOW* window);

// Высота главного окна в символах
const int wnd_h = 40;
// Ширина главного окна в символах
const int wnd_w = 120;

int main(int argc, char ** argv){

    // Строка с полным путем
    char cwd[PATH_MAX];
    // Получаю строку с полным путем и указатель на нее
    char* cwd_ptr = getcwd(cwd, sizeof(cwd));
    if (cwd_ptr == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    // Указатель на окно с рамкой
    WINDOW* wnd;
    // Указатель на окно внутри рамки (чтобы не затереть рамку)
    WINDOW* subwnd;
    // Запуск бпблиотеки
    initscr();
    // Передаю обработчик сигнала SIGWINCH
    signal(SIGWINCH, sig_winch);
    // Не буферизированный ввод (чтобы не вводить Enter после символа)
    cbreak();
    // Отслеживание нажатий
    keypad(stdscr, TRUE);
    // Курсор не видно
    curs_set(FALSE);
    // Включаю цвета
    start_color();
    refresh();
    // Создаю цветовые пары
    // Для полного пути
    init_pair(10, COLOR_BLACK, COLOR_CYAN);
    // Для директорий и файлов
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);

    // Создаю главное окно
    wnd = newwin(wnd_h, wnd_w, 0, 0);
    // Создаю в нем рамку
    box(wnd, '|', '-');
    // Обновляю окно
    wrefresh(wnd);

    // Создаю подокно для текста, чтобы он не затирал рамку главного окна
    subwnd = derwin(wnd, wnd_h-2, wnd_w-2, 1, 1);

    // Указатель на массив структур с информацией о записях в директории
    struct dirent** namelist = NULL;
    // Сканирую и вывожу на экран
    int count = setDirInfo(cwd, &namelist, subwnd);

    // Считываю нажатия клавиш
    int ch;
    int line = 1;
    while (ch != 'q') {

        ch = getch();
        switch(ch) {
            case KEY_UP: // Page Up
                // Если строка не 0 (не полный путь), перейти на предыдущую
                if ((line-1) > 0){
                    line--;
                    // Выделяю строку
                    selectLine(line, namelist, subwnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(line+1, namelist, subwnd);
                }
                break;
            case KEY_DOWN: // Page Down
                // Если строка не последняя, перейти на следующую
                if ( ((line+1) <= count-1) ){
                    line++;
                    // Выделяю строку
                    selectLine(line, namelist, subwnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(line-1, namelist, subwnd);
                }
                break;
            case '\n': // Enter
                // Если строка это директория 
                if (namelist[line]->d_type == DT_DIR){
                    // Если строка первая ('../')
                    if (line == 1){ // '../'
                        int i = strlen(cwd);
                        // Затираю символы до ближайшего '/'
                        while (cwd[i] != '/') {
                            cwd[i] = '\0';
                            i--;
                        }
                        cwd[i] = '\0';
                    // Если строка не первая
                    }else{
                        // Добавляю к пути имя директории и '/'
                        strcat(cwd, "/");
                        strcat(cwd, namelist[line]->d_name);
                    }

                    // Удаляю старую информацию о директории
                    for (int i = 0; i < count; i++)
                        free(namelist[i]);
                    free(namelist);
                    namelist = NULL;

                    // Добавляю информацию о текущей директории
                    line = 1;
                    count = setDirInfo(cwd, &namelist, subwnd);
                }   
                break;
            default:
                break;
        }
    }

    // Удаляю подокно
    delwin(subwnd);
    // Удаляю главное окно
    delwin(wnd);
    // Выключаю быблиотеку
    endwin();

    // Удаляю информацию о текущей директории
    for (int i = 0; i < count; i++)
        free(namelist[i]);
     free(namelist);

    return 0;
}

void writeLine(int line, struct dirent** namelist, WINDOW* window){
    wmove(window, line, 0);  // Перемещаюсь в начало строки 0
    wclrtoeol(window);      // Очищаю строку от текущей позиции до конца строки
    // Записываю в строку line имя записи в директории
    mvwprintw(window, line, 1, "%s", (const char*)(namelist[line]->d_name));
    // Если строка-директория, прибавляю '/' к имени
    if (namelist[line]->d_type == DT_DIR){
         wprintw(window, "/");
    }
}

void selectLine(int line, struct dirent** namelist, WINDOW* window){
    // Включаю атрибут (цветовая пара 1)
    wattron(window, COLOR_PAIR(1));
    // Пишу строку
    writeLine(line, namelist, window);
    // Отключаю отрибут
    wattroff(window, COLOR_PAIR(1));
    // Обновляю подокно
    wrefresh(window);
}

void deSelectLine(int line, struct dirent** namelist, WINDOW* window){
    // Отключаю атрибут цветовой пары 1
    wattroff(window, COLOR_PAIR(1));
    // Пишу строку
    writeLine(line, namelist, window);
    // Обновляю подокно
    wrefresh(window);
}


int setDirInfo(char* pathname, struct dirent*** namelist, WINDOW* window){
    // Сканирую директорию в **namelist и получаю количество файлов
    int count = scandir((const char*)(pathname), namelist, NULL, alphasort);
    if (count < 0) {
        perror("scandir error");
        exit(EXIT_FAILURE);
    }
    // Включаю атрибут цветовой пары 10 для полного пути
    wattron(window, COLOR_PAIR(10));
    // Перемещаюсь в начало строки 0
    wmove(window, 0, 0);
    // Очищаю строку от текущей позиции до конца строки
    wclrtoeol(window);
    // Пишу строку (имя полного пути и добавляю '/' в конце)
    mvwprintw(window, 0, 1, "%s", (const char*)pathname);
    wprintw(window, "/");
    // Отключаю атрибут цветовой пары 10
    wattroff(window, COLOR_PAIR(10));
    // Обновляю подокно
    wrefresh(window);   

    // Пишу и выделяю строку 1 (..) + '/'
    selectLine(1, *namelist, window);
    // Пишу остальные строки и обновляю подокно
    // namelist[0] - директория текущая '.', я ее не включаю в список
    for (int i = 2; i < count; i++) {
        writeLine(i, *namelist, window);
        wrefresh(window);
    }

    // Очищаю остальные строки до конца окна
    for (int i = count; i < wnd_h; i++){
        wmove(window, i, 0); // Перемещаюсь в начало строки 0
        wclrtoeol(window);  // Очищаю строку от текущей позиции до конца строки
    }
    // Обновляю подокно
    wrefresh(window);

    return count;
}