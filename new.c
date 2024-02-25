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

#include "subnew.h"

#define NUM_PAN 2

// Обработчик сигнала SIGWINCH
void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

const int wnd_h = 30;
const int wnd_w = 120;

int main(){

    struct Panel panels[NUM_PAN];
    for (int i = 0; i < NUM_PAN; i++){
        memset((void*)&panels[i].fullpath, 0, sizeof(panels[i].fullpath));
        panels[i].frame_cwd_wnd = NULL;
        panels[i].text_cwd_wnd = NULL;
        panels[i].frame_wnd = NULL;
        panels[i].text_wnd = NULL;
        panels[i].namelist = NULL;
        panels[i].entry_count = 0;
        panels[i].active_line = 1;
    }

    const int panel_h = wnd_h-2;
    const int panel_w = (int)(1.0 * (wnd_w-2) / NUM_PAN + 1);
    WINDOW* wnd = NULL;

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

    char cwd[PATH_MAX];
    char* cwd_ptr = getcwd(cwd, sizeof(cwd));
    if (cwd_ptr == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_PAN; i++){
        panels[i].frame_cwd_wnd = derwin(wnd, 3, panel_w, 0, 0 + i*panel_w);
        box(panels[i].frame_cwd_wnd, '|', '-');
        wrefresh(panels[i].frame_cwd_wnd);

            panels[i].text_cwd_wnd = derwin(panels[i].frame_cwd_wnd, 1, panel_w-2, 0, i*(panel_w-1));
            //mvwprintw(panels[i].text_cwd_wnd, 1, 1, "%s", "fdfdf546666666666666gfgdfgdhrthrteherter66777777777777777774444444444444444444422222222222666dfd");
            wrefresh(panels[i].text_cwd_wnd);

        panels[i].frame_wnd = derwin(wnd, panel_h, panel_w, 2, 0 + i*panel_w);
        box(panels[i].frame_wnd, '|', '-');
        wrefresh(panels[i].frame_wnd);

            panels[i].text_wnd = derwin(panels[i].frame_wnd, panel_h-2, panel_w-2, 0, i*(panel_w-1));
            //mvwprintw(panels[i].text_wnd, 1, 1, "%s", "fdfdf54666rt777777774444444444444444444422222222222666dfd");
            //mvwprintw(panels[i].text_wnd, 2, 1, "%s", "kek");
            wrefresh(panels[i].text_wnd);

        panels[i].entry_count = setDirInfo(cwd, &panels[i].namelist, panels[i].text_wnd);
    }
    
    int ch;
    int ap_id = 0; // active panel id
    while (ch != 'q') {

        ch = getch();
        switch(ch) {
            case KEY_UP: // Page Up
                // Если строка не 0 (не полный путь), перейти на предыдущую
                if ((panels[ap_id].active_line-1) > 0){
                    panels[ap_id].active_line--;
                    // Выделяю строку
                    selectLine(panels[ap_id].active_line, panels[ap_id].namelist, panels[ap_id].text_wnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(panels[ap_id].active_line+1, panels[ap_id].namelist, panels[ap_id].text_wnd);
                }
                break;
            case KEY_DOWN: // Page Down
                // Если строка не последняя, перейти на следующую
                if ( ((panels[ap_id].active_line+1) <= panels[ap_id].entry_count-1) ){
                    panels[ap_id].active_line++;
                    // Выделяю строку
                    selectLine(panels[ap_id].active_line, panels[ap_id].namelist, panels[ap_id].text_wnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(panels[ap_id].active_line-1, panels[ap_id].namelist, panels[ap_id].text_wnd);
                }
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < NUM_PAN; i++){
        delwin(panels[i].frame_cwd_wnd);
        delwin(panels[i].text_cwd_wnd);
        delwin(panels[i].frame_wnd);
        delwin(panels[i].text_wnd);
    }
    delwin(wnd);
    endwin();


    return 0;
}