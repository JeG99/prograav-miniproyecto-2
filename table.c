#include "table.h"

#include <stdio.h>
#include <stdlib.h>

const int CELL_LENGTH = 64;

db_table *createTable(char **columns, int row_space) {
    db_table *table = malloc(sizeof(db_table));
    int columnSize = sizeof(columns) / sizeof(columns[0]);

    table->columns = columns;
    table->rows = malloc(sizeof(char **) * row_space);
    for (int i = 0; i < row_space; i++) {
        table->rows[i] = malloc(sizeof(char *) * columnSize);
        for (int j = 0; j < columnSize; j++) {
            table->rows[i][j] = malloc(CELL_LENGTH);
        }
    }

    table->lastRow = 0;
    matrix_shape shape;
    shape.cols = columnSize;
    shape.rows = row_space;
    table->shape = shape;

    return table;
}