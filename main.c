// Creado para pruebas *
#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "table.h"

int main() {
    char* columns[3] = {"Fecha",
                        "Hora",
                        "Nombre"};

    db_table* my_table = createTable(columns, 3, 5);

    my_table->rows[0][1] = "16:00";

    //printf("Second : %s\n", my_table->rows[0][1]);
    //printf("Column : %s\n", my_table->columns[1]);
    //printf("Shape: (%d, %d)\n", my_table->shape.rows, my_table->shape.cols);

    db_table* parsed = parse("table_1.txt");
    //printf("%s %d\n", parsed->columns[0], parsed->lastRow);
    //printTable(parsed);

    char* selection[] = {"c1", "c3", "c4"};
    db_table* selected = Select(parsed, selection, 3);
    printTable(selected);

    char* insertion[] = {"ins1", "ins2", "ins3"};
    Insert(&selected, insertion);
    printTable(selected);

    //printTable(parsed);

    printTable(selected);

    db_table* where = Where(selected, "c4", "ew");
    printTable(where);

    //printTable(parsed);
}
