#ifndef DB_H
#define DB_H

#include "table.h"

db_table* parse(char* filename);
db_table* Select(db_table* o_table, char** columns, int columnSize);
<<<<<<< HEAD
void Insert(db_table** o_table, char** insertion);
=======

db_table* Where(db_table* o_table, char* column, char* val);
>>>>>>> add where function

#endif /* DB_H */
