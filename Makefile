
server: db.c table.c test_server.c
	gcc test_server.c table.c db.c -o server

client: db.c table.c client.c
	gcc client.c table.c db.c -o client

main: main.c table.c db.c
	gcc main.c table.c db.c -o main