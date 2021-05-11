// Creado para pruebas *
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "table.h"

int main() {
    char* columns[3] = {"Fecha", "Hora", "Nombre"};

    db_table* my_table = createTable(columns, 3, 5);

    my_table->rows[0][1] = "16:00";
    // printf("Second : %s\n", my_table->rows[0][1]);
    // printf("Column : %s\n", my_table->columns[1]);
    // printf("Shape: (%d, %d)\n", my_table->shape.rows, my_table->shape.cols);

    db_table* parsed = parse("table_1.txt");
    // printf("%s %d\n", parsed->columns[0], parsed->lastRow);
    // printTable(parsed);

    char* selection[] = {"c1", "c3", "c4"};
    db_table* selected = Select(parsed, selection, 3);
    printTable(selected);

    db_table* where = Where(selected, "c4", "ew");
    printTable(where);

    // Join
    db_table* joinA = parse("join_test_a.csv");
    db_table* joinB = parse("join_text_b.csv");

    printf("JOIN TEST\n");
    db_table* joinTable = Join(joinA, joinB);

    printf("JOIN A\n");
    printTable(joinA);
    printf("JOIN B\n");
    printTable(joinB);

    printTable(joinTable);

    saveTable("prueb.csv", joinTable);

    db_table* quer = execute("select:c1,c2,c3:from:join_test_a.csv"); 

    if (quer) {
        printTable(quer);
    }

}
