#ifndef DB_H
#define DB_H

#include "table.h"

db_table* parse(char* filename);
db_table* Select(db_table* o_table, char** columns, int columnSize);
db_table* Insert(db_table* o_table, char** insertion);

#endif /* DB_H */