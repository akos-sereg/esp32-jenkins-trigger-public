#include "include/utils.h"

char *get_line(char *text, char *pattern) {
    char *line;

    line = strtok(text, "\n");
    while(line != NULL) {
        line = strtok(NULL, "\n");
        if (strstr(line, pattern) != NULL) {
            return line;
        }
    }

    return NULL;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result;
    char *ins;
    char *tmp;
    int len_rep;
    int len_with;
    int len_front;
    int count;

    if (!orig || !rep) {
	return NULL;
    }

    len_rep = strlen(rep);
    if (len_rep == 0) {
	return NULL;
    }

    if (!with) {
	with = "";
    }
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
	ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result) {
	return NULL;
    }

    while (count--) {
	ins = strstr(orig, rep);
	len_front = ins - orig;
	tmp = strncpy(tmp, orig, len_front) + len_front;
	tmp = strcpy(tmp, with) + len_with;
	orig += len_front + len_rep;
    }

    strcpy(tmp, orig);
    return result;
}