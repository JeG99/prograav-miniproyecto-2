#ifndef TABLE_H
#define TABLE_H

typedef struct {
    int rows;
    int cols;
} matrix_shape;

typedef struct {
    char **columns;      // lista de columnas de la tabla
    char ***rows;        // lista de filas de la tabla
    int lastRow;         // primera fila sin usar de la tabla
    matrix_shape shape;  // dimensiones de la tabla
} db_table;

const int CELL_LENGTH;
const int DEFAULT_ROW_SPACE;

/**
 * Crea un objeto db_table a partir de una lista de columnas y el espacio a
 * alojar para las filas
 * 
 * @param columns lista de columnas de la tabla
 * @param columnSize tamaño de la lista de columnas
 * @param rowSpace la cantidad de filas que se alojan inicialmente
 * 
 * @return tabla creada
 */
db_table *createTable(char **columns, int columnSize, int rowSpace);

/**
 * Copia una tabla
 * 
 * @param table apuntador a la tabla que se va a copiar
 * @return tabla nueva con los datos de la copia
 */
db_table *copyTable(db_table *table);

/**
 * (Para depurar) Imprime los contenidos de una tabla
 * 
 * @param table tabla a imprimir
 */
void printTable(db_table *table);

#endif /* TABLE_H */