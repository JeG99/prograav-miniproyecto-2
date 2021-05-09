typedef struct {
    int rows;
    int cols;
} matrix_shape;

typedef struct {
    char **columns;
    char ***rows;
    int lastRow;
    matrix_shape shape;
} db_table;

const int CELL_LENGTH;

db_table *createTable(char **columns, int row_space);