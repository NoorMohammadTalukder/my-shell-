#ifndef EXEC_H
#define EXEC_H

char *find_executable(const char *cmd); //find full path of cmd 
int run_foreground(const char *fullpath, char *const argv[]); //run command in foreground

#endif