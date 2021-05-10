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

db_table* Select(db_table* o_table, char** columns, int columnSize) {
    db_table* table = malloc(sizeof(db_table));
    table->columns = columns;

    matrix_shape shape;
    shape.rows = o_table->shape.rows;
    shape.cols = columnSize;
    table->shape = shape;
    table->lastRow = o_table->lastRow;

    table->rows = malloc(sizeof(char**) * shape.rows);

    // Copia las filas de la tabla original
    for (int i = 0; i < table->lastRow; i++) {
        table->rows[i] = malloc(sizeof(char*) * shape.cols);

        // Itera cada columna del select
        for (int col_idx = 0; col_idx < shape.cols; col_idx++) {
            char* col = columns[col_idx];
            for (int j = 0; j < o_table->shape.cols; j++) {
                // Inserta el valor de la columna en la fila si esta en el select
                if (strcmp(col, o_table->columns[j]) == 0) {
                    table->rows[i][col_idx] = malloc(CELL_LENGTH);
                    strcpy(table->rows[i][col_idx], o_table->rows[i][j]);
                    break;
                }
            }
        }
    }

    return table;
}

void Insert(db_table** o_table, char** insertion) {
    (*o_table)->rows[(*o_table)->lastRow] = malloc(sizeof(char*) * (*o_table)->shape.cols);
    for (int i = 0; i < (*o_table)->shape.cols; i++) {
        (*o_table)->rows[(*o_table)->lastRow][i] = malloc(CELL_LENGTH);
        (*o_table)->rows[(*o_table)->lastRow][i] = insertion[i];
        printf("%s \n", (*o_table)->rows[(*o_table)->lastRow][i]);
    }
    (*o_table)->lastRow++;
}