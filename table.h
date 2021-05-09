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
