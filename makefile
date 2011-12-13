all:  bin/Anelosimus.Eximius

/tmp/url.so: c/url.so
	      cp c/url.so /tmp
c/url.so: c/url.c
	  cd c; make ; cd .. 
CreateTables: /tmp/url.so sql/url.sql sql/CreateTables.sql 
	psql -f sql/url.sql
	psql -f sql/CreateTables.sql 

bin/Anelosimus.Eximius : ecpg/Anelosimus.Eximius.pgc 
	cd ecpg
	ecpg -t -I/usr/include/postgresql -I/usr/include/curl ecpg/Anelosimus.Eximius.pgc
	gcc -g -o bin/Anelosimus.Eximius -I /usr/include/postgresql/ ecpg/Anelosimus.Eximius.c -lecpg -lpq -lcurl

