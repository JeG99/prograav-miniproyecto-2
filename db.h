#ifndef DB_H
#define DB_H

#include "table.h"

db_table* parse(char* filename);
db_table* select(db_table* o_table, char** columns, int columnSize);

#endif /* DB_H */