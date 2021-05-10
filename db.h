#ifndef DB_H
#define DB_H

#include "table.h"

db_table* parse(char* filename);
db_table* Select(db_table* o_table, char** columns, int columnSize);
db_table* Join(db_table* table_a, db_table* table_b);
void Insert(db_table** o_table, char** insertion);
db_table* Where(db_table* o_table, char* column, char* val);

#endif /* DB_H */
