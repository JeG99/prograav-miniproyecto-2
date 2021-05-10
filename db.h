#ifndef DB_H
#define DB_H

#include "table.h"

db_table* parse(char* filename);
db_table* Select(db_table* o_table, char** columns, int columnSize);

db_table* Where(db_table* o_table, char* column, char* val);

#endif /* DB_H */