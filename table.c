#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int CELL_LENGTH = 64;
const int DEFAULT_ROW_SPACE = 50;

db_table *createTable(char **columns, int columnSize, int rowSpace) {
    db_table *table = malloc(sizeof(db_table));

    table->columns = columns;
    table->rows = malloc(sizeof(char **) * rowSpace);
    for (int i = 0; i < rowSpace; i++) {
        table->rows[i] = malloc(sizeof(char *) * columnSize);
        for (int j = 0; j < columnSize; j++) {
            table->rows[i][j] = malloc(CELL_LENGTH);
        }
    }

    table->lastRow = 0;
    matrix_shape shape;
    shape.cols = columnSize;
    shape.rows = rowSpace;
    table->shape = shape;

    return table;
}

db_table *copyTable(db_table *table) {
    db_table *copy_table = malloc(sizeof(db_table));
    int columnSize = table->shape.cols;
    int rowSpace = table->shape.rows;

    copy_table->columns = malloc(sizeof(char *) * columnSize);
    copy_table->rows = malloc(sizeof(char **) * rowSpace);
    for (int i = 0; i < rowSpace; i++) {
        table->rows[i] = malloc(sizeof(char *) * columnSize);
        for (int j = 0; j < columnSize; j++) {
            strcpy(copy_table->rows[i][j], table->rows[i][j]);
        }
    }

    copy_table->lastRow = table->lastRow;
    matrix_shape shape;
    shape.cols = columnSize;
    shape.rows = rowSpace;
    copy_table->shape = shape;

    return table;
}

void printTable(db_table *table) {
    printf("COLUMNS: %d\n", table->shape.cols);
    for (int i = 0; i < table->shape.cols; i++) {
        printf("%s\t\t", table->columns[i]);
    }
    printf("\n---------------------------------------\n");
    for (int i = 0; i < table->lastRow; i++) {
        for (int j = 0; j < table->shape.cols; j++) {
            printf("%s\t\t", table->rows[i][j]);
        }
        printf("\n");
    }
}