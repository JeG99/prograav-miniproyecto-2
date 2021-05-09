// Creado para pruebas * 
#include <stdio.h>
#include <stdlib.h>

#include "table.h"

int main() {
    char* columns[3] = {"Fecha",
                        "Hora",
                        "Nombre"};

    db_table* my_table = createTable(columns, 5);

    my_table->rows[0][1] = "16:00";
    printf("Second : %s\n", my_table->rows[0][1]);
    printf("Column : %s\n", my_table->columns[1]);
    printf("Shape: (%d, %d)\n", my_table->shape.rows, my_table->shape.cols);
}