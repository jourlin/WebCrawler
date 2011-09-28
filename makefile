all:  bin/Anelosimus.Eximius

CreateTables: sql/CreateTables.sql 
	psql -f sql/CreateTables.sql 

bin/Anelosimus.Eximius : ecpg/Anelosimus.Eximius.pgc
	cd ecpg
	ecpg -t -I/usr/include/postgresql -I/usr/include/curl ecpg/Anelosimus.Eximius.pgc
	gcc -g3 -o bin/Anelosimus.Eximius -I /usr/include/postgresql/ ecpg/Anelosimus.Eximius.c -lecpg -lpq -lcurl

