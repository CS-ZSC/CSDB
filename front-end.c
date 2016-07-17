#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csdb.h"

static char buffer[80];

struct command read_command()
{
    buffer[0] = 0;
    fflush(stdin);
    struct command cmd;
    fgets(buffer, sizeof(buffer), stdin);

    if (! buffer[0]) {
        cmd.id = EMPTY;
        return cmd;
    }
    
    char *cmd_s = strtok(buffer, " \t\n");

    if (!cmd_s) {
        cmd.id = -1;
    } else if (!strcmp(cmd_s, "EXIT")) {
        cmd.id = EXIT;
    } else if (!strcmp(cmd_s, "INFO")) {
        cmd.id = INFO;
    } else if (!strcmp(cmd_s, "CREATE")) {
        cmd.id = CREATE;
        cmd.arg = strtok(NULL, " \t\n");
    } else if (!strcmp(cmd_s, "OPEN")) {
        cmd.id = OPEN;
        cmd.arg = strtok(NULL, " \t\n");
    } else if (!strcmp(cmd_s, "NEW")) {
        cmd.id = NEW;
    } else if (!strcmp(cmd_s, "LIST")) {
        cmd.id = LIST;
    } else {
        cmd.id = -1;
    }

    return cmd;
}

int main(int argc, char *argv[])
{
    for (;;) {
        printf("CSDB> ");
        struct command cmd = read_command();
        process_command(cmd);    
    }
}
