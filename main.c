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

// Обработчик сигнала SIGWINCH
void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}
const int num_panels = 2;

const int cwd_wnd_h = 2;
const int wnd_h = 30;
const int wnd_w = 120;

const int panel_wnd_h = wnd_h-2;
const int panel_wnd_w = (int)(1.0 * (wnd_w-2) / num_panels + 1);

int main(){

    struct Panel panels[num_panels];
    for (int i = 0; i < num_panels; i++){
        memset((void*)&panels[i].fullpath, 0, sizeof(panels[i].fullpath));
        panels[i].frame_cwd_wnd = NULL;
        panels[i].text_cwd_wnd = NULL;
        panels[i].frame_wnd = NULL;
        panels[i].text_wnd = NULL;
        panels[i].namelist = NULL;
        panels[i].entry_count = 0;
        panels[i].active_line = 0;
    }
    WINDOW* wnd = NULL;

    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
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

    for (int i = 0; i < num_panels; i++){
        int cwd_rw_h = cwd_wnd_h+2;
        int cwd_rw_w = panel_wnd_w;
        int cwd_ry = 0;
        int cwd_rx = i*panel_wnd_w;
        panels[i].frame_cwd_wnd = derwin(wnd, cwd_rw_h, cwd_rw_w, cwd_ry, cwd_rx);
        box(panels[i].frame_cwd_wnd, '|', '-');
        wrefresh(panels[i].frame_cwd_wnd);

            panels[i].text_cwd_wnd = derwin(panels[i].frame_cwd_wnd, cwd_wnd_h, panel_wnd_w-2, 1, 1);
            //mvwprintw(panels[i].text_cwd_wnd, 0, 0, "%s", "fffffffffdddddddddaaaaaa88888888888888888888888888aaaaaggggggg");
            wrefresh(panels[i].text_cwd_wnd);

        int txt_rw_h = panel_wnd_h-cwd_wnd_h+1;
        int txt_rw_w = panel_wnd_w;
        int txt_ry = cwd_wnd_h+1;
        int txt_rx = i*panel_wnd_w;
        panels[i].frame_wnd = derwin(wnd, txt_rw_h, txt_rw_w, txt_ry, txt_rx);
        box(panels[i].frame_wnd, '|', '-');
        wrefresh(panels[i].frame_wnd);

            panels[i].text_wnd = derwin(panels[i].frame_wnd, txt_rw_h-2, txt_rw_w-2, 1, 1);
            //mvwprintw(panels[i].text_wnd, 0, 0, "%s", "fdfdf54666rt7777777744444444444444444444222267567567666dfd");
            wrefresh(panels[i].text_wnd);

        if (strncpy(panels[i].fullpath, cwd, sizeof(cwd)) == NULL){
            perror("Error cpy fullpath");
            exit(-1);
        }
        panels[i].entry_count = setDirInfo(panels[i].fullpath, &panels[i].namelist, panels[i].text_wnd);
        if (i == 0)
            setPathInfo(panels[i].fullpath, panels[i].text_cwd_wnd, 10, 1);
        else
            setPathInfo(panels[i].fullpath, panels[i].text_cwd_wnd, 0, 1);
        wrefresh(panels[i].text_wnd);
    }
    
    int ch;
    int ap_id = 0; // active panel id
    while (ch != 'q') {

         ch = getch();
        switch(ch) {
            case KEY_UP: // Page Up
                // Если строка не 0 (не полный путь), перейти на предыдущую
                if ((panels[ap_id].active_line) > 0){
                    panels[ap_id].active_line--;
                    // Выделяю строку
                    selectLine(panels[ap_id].active_line, panels[ap_id].namelist, panels[ap_id].text_wnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(panels[ap_id].active_line+1, panels[ap_id].namelist, panels[ap_id].text_wnd);
                }
                break;
            case KEY_DOWN: // Page Down
                // Если строка не последняя, перейти на следующую
                if ( ((panels[ap_id].active_line) < panels[ap_id].entry_count-2) ){
                    panels[ap_id].active_line++;    
                    // Выделяю строку
                    //writeLine(panels[ap_id].active_line, panels[ap_id].namelist, panels[ap_id].text_wnd);
                    //wrefresh(panels[ap_id].text_wnd);
                    selectLine(panels[ap_id].active_line, panels[ap_id].namelist, panels[ap_id].text_wnd);
                    // Снимаю выделение с предыдущей строки
                    deSelectLine(panels[ap_id].active_line-1, panels[ap_id].namelist, panels[ap_id].text_wnd);
                }
                break;
            case '\n': // Enter
                // Если строка это директория 
                if (panels[ap_id].namelist[panels[ap_id].active_line+1]->d_type == DT_DIR){
                    // Если строка первая ('../')
                    if (panels[ap_id].active_line == 0){ // '../'

                        int i = strlen(panels[ap_id].fullpath);
                        // Затираю символы до ближайшего '/'
                        panels[ap_id].fullpath[i] = '\0';
                        i--;
                        while (panels[ap_id].fullpath[i] != '/') {
                            panels[ap_id].fullpath[i] = '\0';
                            i--;
                        }
                        // if next fullpath is not "/"
                        if (strcmp((const char*)(panels[ap_id].fullpath), "/") != 0){
                            panels[ap_id].fullpath[i] = '\0';
                            setPathInfo(panels[ap_id].fullpath, panels[ap_id].text_cwd_wnd, 10, 1);
                        }else{
                            setPathInfo(panels[ap_id].fullpath, panels[ap_id].text_cwd_wnd, 10, 0);
                        }
                    // Если строка не первая
                    }else{
                        // Добавляю к пути имя директории и '/'
                        if (strcmp((const char*)(panels[ap_id].fullpath), "/") != 0)
                            strcat(panels[ap_id].fullpath, "/");
                        strcat(panels[ap_id].fullpath, panels[ap_id].namelist[panels[ap_id].active_line+1]->d_name);
                        setPathInfo(panels[ap_id].fullpath, panels[ap_id].text_cwd_wnd, 10, 1);
                    }

                    // Удаляю старую информацию о директории
                    for (int i = 0; i < panels[ap_id].entry_count; i++)
                        free(panels[ap_id].namelist[i]);
                    free(panels[ap_id].namelist);
                    panels[ap_id].namelist = NULL;

                    // Добавляю информацию о текущей директории
                    panels[ap_id].entry_count = 0;
                    panels[ap_id].active_line = 0;
                    panels[ap_id].entry_count = setDirInfo(panels[ap_id].fullpath, &panels[ap_id].namelist, panels[ap_id].text_wnd);

                }       
                break;
            case '\t':
                int prev_ap_id = ap_id; 
                ap_id++;
                if (ap_id > (num_panels-1))
                    ap_id = 0;
                setPathInfo(panels[ap_id].fullpath, panels[ap_id].text_cwd_wnd, 10, 1);
                setPathInfo(panels[prev_ap_id].fullpath, panels[prev_ap_id].text_cwd_wnd, 0, 1);
                wrefresh(panels[ap_id].text_wnd);
                break;
            default:
                break;
        }
        }

    for (int i = 0; i < num_panels; i++){
        delwin(panels[i].frame_cwd_wnd);
        delwin(panels[i].text_cwd_wnd);
        delwin(panels[i].frame_wnd);
        delwin(panels[i].text_wnd);
    }
    delwin(wnd);
    endwin();


    return 0;
}