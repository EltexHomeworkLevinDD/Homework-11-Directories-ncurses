#ifndef SUBNEW_H
#define SUBNEW_H

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
#include <linux/limits.h>


struct Panel{
    int entry_count;
    int active_line;
    struct dirent** namelist;
    WINDOW* frame_cwd_wnd;
    WINDOW* text_cwd_wnd;
    WINDOW* frame_wnd;
    WINDOW* text_wnd;
    char fullpath[PATH_MAX];
};

void writeLine(int line, struct dirent** namelist, WINDOW* window);
void selectLine(int line, struct dirent** namelist, WINDOW* window);
void deSelectLine(int line, struct dirent** namelist, WINDOW* window);
int setDirInfo(char* pathname, struct dirent*** namelist, WINDOW* window);
void setPathInfo(char* pathname, WINDOW* window, int color_pair);

#endif