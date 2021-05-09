#include "db.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* trim(char* str) {
    char* end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

db_table* parse(char* filename) {
    FILE* file = fopen(filename, "r");

    char** columns = malloc(sizeof(char*) * 30);
    db_table* table;
    int columnCount = 0;

    if (file != NULL) {
        char buffer[1024];

        int row = 0;
        int column = 0;

        while (fgets(buffer, 1024, file)) {
            if (row == 0) {
                char* value = strtok(buffer, ", ");

                while (value) {
                    columns[columnCount] = malloc(sizeof(char) * CELL_LENGTH);
                    strcpy(columns[columnCount], trim(value));
                    columnCount++;
                    value = strtok(NULL, ", ");
                    column++;
                }

                table = createTable(columns, columnCount, DEFAULT_ROW_SPACE);
            } else {
                column = 0;
                char* value = strtok(buffer, ", ");

                while (value) {
                    if (table->lastRow == table->shape.rows) {
                        // TODO: resize table
                    } else {
                        strcpy(table->rows[row - 1][column], trim(value));
                        table->lastRow = row;
                    }

                    value = strtok(NULL, ", ");
                    column++;
                }
            }
            row++;
        }

    } else {
        printf("El archivo %s no existe", filename);
        exit(0);
    }

    if (file) fclose(file);

    return table;
}
