#include "subnew.h"

extern const int wnd_h;
extern const int cwd_wnd_h;
extern const int panel_wnd_h;

void writeLine(int line, struct dirent** namelist, WINDOW* window){
    wmove(window, line, 0);  // Перемещаюсь в начало строки 0
    wclrtoeol(window);      // Очищаю строку от текущей позиции до конца строки
    // Записываю в строку line имя записи в директории
    mvwprintw(window, line, 1, "%s", (const char*)(namelist[line+1]->d_name));
    // Если строка-директория, прибавляю '/' к имени
    if (namelist[line+1]->d_type == DT_DIR){
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

void setPathInfo(char* pathname, WINDOW* window, int color_pair, int add_slash){
    // Включаю атрибут цветовой пары для полного пути
    wattron(window, COLOR_PAIR(color_pair));
    for (int i = 0; i < cwd_wnd_h; i++){
        // Перемещаюсь в начало строки 0
        wmove(window, i, 0);
        // Очищаю строку от текущей позиции до конца строки
        wclrtoeol(window);
    }
    // Пишу строку (имя полного пути и добавляю '/' в конце)
    mvwprintw(window, 0, 0, "%s", (const char*)pathname);
    if (add_slash)
        wprintw(window, "/");
    // Отключаю атрибут цветовой пары
    wattroff(window, COLOR_PAIR(color_pair));
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

    // Пишу и выделяю строку 1 (..) + '/'
    selectLine(0, *namelist, window);
    // Пишу остальные строки и обновляю подокно
    // namelist[0] - директория текущая '.', я ее не включаю в список
    for (int i = 1; i < count-1; i++) {
        writeLine(i, *namelist, window);
        wrefresh(window);
    }

    // Очищаю остальные строки до конца окна
    for (int i = count-1; i < panel_wnd_h-2; i++){
        wmove(window, i, 0); // Перемещаюсь в начало строки 0
        wclrtoeol(window);  // Очищаю строку от текущей позиции до конца строки
    }
    // Обновляю подокно
    wrefresh(window);

    return count;
}