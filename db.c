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

void saveTable(char* filename, db_table* table) {
    FILE* file = fopen(filename, "w");

    matrix_shape shape = table->shape;

    if (file != NULL) {
        // Guardar columnas
        for (int i = 0; i < shape.cols; i++) {
            fprintf(file, "%s,", table->columns[i]);
        }
        fprintf(file, "\n");

        // Guardar filas
        for (int i = 0; i < table->lastRow; i++) {
            for (int j = 0; j < shape.cols; j++) {
                fprintf(file, "%s,", table->rows[i][j]);
            }
            fprintf(file, "\n");
        }

    } else {
        printf("El archivo %s no existe", filename);
        exit(0);
    }

    if (file) fclose(file);
}

db_table* execute(char* o_query) {
    char* selectColumns[100];
    char* whereClause[2];
    int whereIdx = 0;
    int selectSize;
    int stepMarks[] = {0, 0, 0, 0};

    char query[2000];
    strcpy(query, o_query);

    char* end_str;
    char* token = strtok_r(query, ":", &end_str);

    db_table* result_table;
    db_table* from_table = NULL;
    db_table* join_table = NULL;

    while (token != NULL) {
        if (strcmp(token, "select") == 0) {
            for (int i = 0; i < 4; i++) {
                if (stepMarks[i] != 0) {
                    printf("Invalid query\n");
                    return NULL;
                }
            }
            token = strtok_r(NULL, ":", &end_str);
            char* end_token;
            char* subtoken = strtok_r(token, ",", &end_token);
            int col_idx = 0;

            while (subtoken != NULL) {
                selectColumns[col_idx] = malloc(sizeof(char*) * CELL_LENGTH);
                strcpy(selectColumns[col_idx], trim(subtoken));
                col_idx++;
                subtoken = strtok_r(NULL, ",", &end_token);
            }

            selectSize = col_idx;
            stepMarks[0] = 1;
        } else if (strcmp(token, "from") == 0) {
            if (stepMarks[0] != 1) {
                printf("Invalid query\n");
                return NULL;
            }
            for (int i = 1; i < 4; i++) {
                if (stepMarks[i] != 0) {
                    printf("Invalid query\n");
                    return NULL;
                }
            }
            token = strtok_r(NULL, ":", &end_str);

            from_table = parse(token);
            stepMarks[1] = 1;
        } else if (strcmp(token, "join") == 0) {
            if (stepMarks[0] != 1 || stepMarks[1] != 1) {
                printf("Invalid query\n");
                return NULL;
            }
            for (int i = 2; i < 4; i++) {
                if (stepMarks[i] != 0) {
                    printf("Invalid query\n");
                    return NULL;
                }
            }
            token = strtok_r(NULL, ":", &end_str);

            stepMarks[2] = 1;
            join_table = parse(token);
        } else if (strcmp(token, "where") == 0) {
            if (stepMarks[0] != 1 || stepMarks[1] != 1) {
                printf("Invalid query\n");
                return NULL;
            }

            token = strtok_r(NULL, ":", &end_str);
            char* end_token;
            char* subtoken = strtok_r(token, "=", &end_token);
            char* compares[10];
            int idx = 0;
            while (subtoken) {
                if (idx >= 2) {
                    printf("Invalid query\n");
                    return NULL;
                }
                whereClause[idx] = malloc(sizeof(char*) * CELL_LENGTH);
                strcpy(whereClause[idx], trim(subtoken));
                idx++;

                subtoken = strtok_r(NULL, "=", &end_token);
            }
            whereIdx = idx;
            stepMarks[3] = 1;
        } else {
            printf("Invalid query\n");
            return NULL;
        }

        token = strtok_r(NULL, ":", &end_str);
    }

    if (!from_table) return NULL;

    if (join_table) {
        result_table = Join(from_table, join_table);
    } else {
        result_table = from_table;
    }

    if (!result_table) {
        printf("Invalid query\n");
        return NULL;
    }
    
    result_table = Select(result_table, selectColumns, selectSize);

    if (!result_table) {
        printf("Invalid query\n");
        return NULL;
    }

    if (whereIdx == 2) {
        result_table = Where(result_table, whereClause[0], whereClause[1]);
    }

    if (!result_table) {
        printf("Invalid query\n");
        return NULL;
    }

    return result_table;
}

db_table* Select(db_table* o_table, char** columns, int columnSize) {
    if (columnSize > o_table->shape.cols) {
        printf("Invalid select\n");
        return NULL;
    }

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
                // Inserta el valor de la columna en la fila si esta en el
                // select
                if (strcmp(col, o_table->columns[j]) == 0) {
                    table->rows[i][col_idx] = malloc(CELL_LENGTH);
                    strcpy(table->rows[i][col_idx], o_table->rows[i][j]);
                    break;
                }

                if (j == o_table->shape.cols - 1) {
                    printf("Invalid select\n");
                    return NULL;
                }
            }
        }
    }

    return table;
}

db_table* Join(db_table* table_a, db_table* table_b) {
    // Busca columna comun
    int taCommonColIdx = -1, tbCommonColIdx = -1;

    for (int i = 0; i < table_a->shape.cols; i++) {
        for (int j = i; j < table_b->shape.cols; j++) {
            if (strcmp(table_a->columns[i], table_b->columns[j]) == 0) {
                taCommonColIdx = i;
                tbCommonColIdx = j;
            }
        }
        if (i == table_a->shape.cols - 1 && taCommonColIdx == -1) {
            return NULL;  // No hay columna para hacer JOIN
        }
    }

    // Dimensiones de la tabla

    matrix_shape shape;
    shape.rows = table_a->shape.rows > table_b->shape.rows
                     ? table_a->shape.rows
                     : table_b->shape.rows;
    shape.cols = table_a->shape.cols + table_b->shape.cols - 1;

    char**  columns = malloc(sizeof(char*) * shape.cols);

    int offset = 0;
    for (int i = 0; i < shape.cols; i++) {
        columns[i] = malloc(sizeof(char) * CELL_LENGTH);

        if (i < table_a->shape.cols) {
            strcpy(columns[i], table_a->columns[i]);
        } else {
            int bColIdx = i - table_a->shape.cols + offset;
            if (tbCommonColIdx != bColIdx) {
                strcpy(columns[i], table_b->columns[bColIdx]);
            } else {
                strcpy(columns[i], table_b->columns[bColIdx + 1]);
                offset++;
            }
        }
    }

    db_table* table = createTable(columns, shape.cols, shape.rows);


    for (int a_i = 0; a_i < table_a->lastRow; a_i++) {
        for (int b_i = 0; b_i < table_b->lastRow; b_i++) {
            if (strcmp(table_a->rows[a_i][taCommonColIdx],
                       table_b->rows[b_i][tbCommonColIdx]) == 0) {
                int offset = 0;
                for (int i = 0; i < shape.cols; i++) {
                    if (i < table_a->shape.cols) {
                        strcpy(table->rows[table->lastRow][i], table_a->rows[a_i][i]);
                    } else {
                        int bColIdx = i - table_a->shape.cols + offset;
                        if (tbCommonColIdx != bColIdx) {
                            strcpy(table->rows[table->lastRow][i], table_b->rows[b_i][bColIdx]);
                        } else {
                            strcpy(table->rows[table->lastRow][i], table_b->rows[b_i][bColIdx + 1]);
                            offset++;
                        }
                    }
                }

                table->lastRow++;
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
    }
    (*o_table)->lastRow++;
}

db_table* Where(db_table* o_table, char* column, char* val) {
    db_table* table = malloc(sizeof(db_table));
    table->columns = o_table->columns;
    
    matrix_shape shape;
    shape.rows = o_table->shape.rows;
    shape.cols = o_table->shape.cols;
    table->shape = shape;
    table->lastRow = o_table->lastRow;

    table->rows = malloc(sizeof(char**) * shape.rows);

    // Copia las filas de la tabla original
    int row = 0;
    for (int i = 0; i < o_table->lastRow; i++) {
        table->rows[row] = malloc(sizeof(char*) * shape.cols);
        for (int j = 0; j < o_table->shape.cols; j++) {
            // Inserta el valor de la columna en la fila si esta en el select
            if (strcmp(column, o_table->columns[j]) == 0) {
                if(strcmp(val, o_table->rows[i][j]) == 0){
                    for(int ii = 0; ii < o_table->shape.cols; ii++){
                        table->rows[row][ii] = malloc(CELL_LENGTH);
                        strcpy(table->rows[row][ii], o_table->rows[i][ii]);
                    }
                    row++;
                }
            }
        }
    }
    table->lastRow = row;
    return table;
}
