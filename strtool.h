#ifndef STRTOOL_H_
#define STRTOOL_H_

#include <string.h>
#include <vector>

char *strip(char *&str);
char *r_strip(char *&str);
char *l_strip(char *&str);
void split(std::vector<char *> &list, char *str, char sep);

#endif
