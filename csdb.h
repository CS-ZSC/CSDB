#ifndef _CSDB_H
#define _CSDB_H

struct record {
    char name[20];
    char rule[20];
    int  points;
};

enum cmd_id {
    EMPTY,
    INFO,
    EXIT,
    CREATE,
    OPEN,
    NEW,
    LIST,
};

struct command {
    enum cmd_id id;	/* Command */
    char *arg;	/* Argument */
};

void process_command(struct command cmd);

#endif /* ! _CSDB_H */
