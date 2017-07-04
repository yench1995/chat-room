#include "strtool.h"
using namespace std;

// remove the \t\n character in the right side
char *r_strip(char* &str)
{
    if (str == NULL)
        return NULL;
    char *p = str + strlen(str) - 1;
    char filter_chs[] = "\t\n";
    while (p >= str && strchr(filter_chs, *p))
        *p-- = 0;
    return str;
}

// remove the \t\n character in the left side
char *l_strip(char* &str)
{
    if (str == NULL)
        return NULL;
    char *p = str;
    char filter_chs[] = "\t\n";
    while (*p && strchr(filter_chs, *p))
        *p++ = 0;
    str = p;
    return str;
}

char *strip(char* &str)
{
    r_strip(str);
    l_strip(str);
    return str;
}

void split(vector<char *> &list, char *str, char sep)
{
    list.clear();
    if (str == NULL)
        return;

    char *begin = str;
    char *end = strchr(begin, sep);
    while (end)
    {
        list.push_back(begin);
        *end++ = 0;
        begin = end;
        end = strchr(begin, sep);
    }
    list.push_back(begin);
    if ((end = strchr(begin, '\n')) != 0)
        *end = 0;
}
