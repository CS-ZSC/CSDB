#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "csdb.h"

static char *filename = NULL;
static FILE *database = NULL;
static unsigned records = 0;

void cmd_info()
{
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
    } else {
        printf("FILE: %s [%u RECORDS]\n", filename, records);
    }
}

void cmd_create(char *fn)
{
    printf("CREATING FILE \"%s\"\n", fn);
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
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
        return;
    }
    
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

void cmd_select(char *arg)
{
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
        return;
    }
    
    if (!arg || !arg[0]) {
        printf("SELECT EXPECTS AN ARGUMENT OF FORMAT <FIELD>=<VALUE>\n");
        return;
    }
    
    char *field = strtok(arg, "=");
    char *value = strtok(NULL, " \n\t");
    
    if (!field || !value) {
        printf("SELECT EXPECTS AN ARGUMENT OF FORMAT <FIELD>=<VALUE>\n");
        return;
    }
    
    if (!strcmp(field, "NAME")) {   /* Select by name */
        printf("%5s%20s%20s%20s\n", "IDX", "NAME", "RULE", "POINTS");
        fseek(database, 0, SEEK_SET);
        int i;
        for (i = 0; i < records; ++i) {
            struct record record;
            fread(&record, sizeof(record), 1, database);
            if (!strcmp(record.name, value)) {
                printf("%5d%20s%20s%20d\n", i, record.name, record.rule, record.points);
            }
        }
    } else if (!strcmp(field, "RULE")) {   /* Select by rule */
        printf("%5s%20s%20s%20s\n", "IDX", "NAME", "RULE", "POINTS");
        fseek(database, 0, SEEK_SET);
        int i;
        for (i = 0; i < records; ++i) {
            struct record record;
            fread(&record, sizeof(record), 1, database);
            if (!strcmp(record.rule, value)) {
                printf("%5d%20s%20s%20d\n", i, record.name, record.rule, record.points);
            }
        }
    } else if (!strcmp(field, "POINTS")) {   /* Select by points */
        printf("%5s%20s%20s%20s\n", "IDX", "NAME", "RULE", "POINTS");
        int points;
        sscanf(value, "%d", &points);
        
        fseek(database, 0, SEEK_SET);
        int i;
        for (i = 0; i < records; ++i) {
            struct record record;
            fread(&record, sizeof(record), 1, database);
            if (record.points == points) {
                printf("%5d%20s%20s%20d\n", i, record.name, record.rule, record.points);
            }
        }
    } else {
        printf("UNRECOGNIZED FIELD: %s\n", field);
    }
}

void cmd_list()
{
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
        return;
    }
    
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

void cmd_delete(char *arg)
{
    if (!database) {
        printf("YOU MUST OPEN OR CREATE A FILE FIRST\n");
        return;
    }
    
    if (!arg || !arg[0]) {
        printf("DELETE EXPECTS THE INDEX OF THE RECORD\n");
        return;
    }
    
    int idx, i;
    sscanf(arg, "%d", &idx);
    
    if (idx < 0 || idx >= records) {
        printf("ERROR INDEX OUT OF RANGE\n");
        return;
    }
    
    struct record record;
    
    for (i = idx + 1; i < records; ++i) {
        fseek(database, i * sizeof(record), SEEK_SET);
        fread(&record, sizeof(record), 1, database);
        fseek(database, (i - 1) * sizeof(record), SEEK_SET);
        fwrite(&record, sizeof(record), 1, database);
    }
    
    --records;
    
    if (ftruncate(fileno(database), records * sizeof(record))) {
        perror("FATAL ERROR FTRUNCATE");
        exit(-1);
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
        case SELECT:
            cmd_select(cmd.arg);
            break;
        case LIST:
            cmd_list();
            break;
        case DELETE:
            cmd_delete(cmd.arg);
            break;
        default:
            printf("UNRECOGNIZED COMMAND\n");
            break;
    }
}
