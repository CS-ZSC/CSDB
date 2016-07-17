#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csdb.h"

static char *filename = NULL;
static FILE *database = NULL;
static unsigned records = 0;

void cmd_info()
{
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
    } else {
        printf("FILE \"%s\"\n", filename);
    }
}

void cmd_create(char *fn)
{
    printf("CREATING FILE %s\n", fn);
    database = fopen(fn, "w+");

    if (!database) {
        perror("ERROR CREATING FILE");
        return;
    }
    
    if (filename) free(filename);
    filename = strdup(fn);
    records = 0;
}

void cmd_open(const char *fn)
{
    printf("OPENING FILE \"%s\"\n", fn);
    database = fopen(fn, "r+");

    if (!database) {
        perror("ERROR OPENING FILE");
        return;
    }
    
    if (filename) free(filename);
    filename = strdup(fn);
    
    fseek(database, 0, SEEK_END);
    records = ftell(database)/sizeof(struct record);
    fseek(database, 0, SEEK_SET);
}

void cmd_new()
{
    struct record record;
    printf("NAME: ");
    scanf("%s", record.name);
    printf("RULE: ");
    scanf("%s", record.rule);
    printf("POINTS: ");
    scanf("%d", &record.points);
    
    while (fgetc(stdin) != '\n');   /* Flush stdin */
    
    fseek(database, records * sizeof(record), SEEK_SET);
    fwrite(&record, sizeof(record), 1, database);
    ++records;
}

void cmd_list()
{
    printf("FILE: %s [%u RECORDS]\n", filename, records);
    printf("%5s%20s%20s%20s\n", "IDX", "NAME", "RULE", "POINTS");

    struct record record;
    fseek(database, 0, SEEK_SET);

    int i;
    for (i = 0; i < records; ++i) {
        fread(&record, sizeof(record), 1, database);
        printf("%5d%20s%20s%20d\n", i, record.name, record.rule, record.points);
    }
}

void process_command(struct command cmd)
{
    switch (cmd.id) {
        case EMPTY:
            printf("\n");
            break;
        case INFO:
            cmd_info();
            break;
        case EXIT:
            exit(0);
        case CREATE:
            cmd_create(cmd.arg);
            break;
        case OPEN:
            cmd_open(cmd.arg);
            break;
        case NEW:
            cmd_new();
            break;
        case LIST:
            cmd_list();
            break;
        default:
            printf("UNRECOGNIZED COMMAND\n");
            break;
    }
}
