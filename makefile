include ./webcrawler.sample.cfg
# Usual path for pg_config :
PG_CONFIG = pg_config
# Or specific :
# PG_CONFIG = /usr/local/pgsql/bin/pg_config
SHAREDIR = $(shell $(PG_CONFIG) --sharedir)

all:  bin/Anelosimus.Eximius
clean:  DropDataBase
	rm bin/Anelosimus.Eximius
	rm $(SHAREDIR)/extension/url.*
DropDataBase:
	dropdb -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) 

extensions: $(SHAREDIR)/extension/url.sql  $(SHAREDIR)/extension/url.so
		
$(SHAREDIR)/extension/url.so:	c/url.so
	cp c/url.so $(SHAREDIR)/extension/url.so
	chown postgres.postgres $(SHAREDIR)/extension/url.so
$(SHAREDIR)/extension/url.sql:	sql/url.sql
	sed "s:_OBJWD_:"$(SHAREDIR)"/extension:g" sql/url.sql > $(SHAREDIR)/extension/url.sql
	chown postgres.postgres $(SHAREDIR)/extension/url.sql
c/url.so: c/url.c
	echo $(SHAREDIR)
	  cd c; make ; cd .. 
CreateDatabase: 
	createdb -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME)
CreateTables: $(SHAREDIR)/extension/url.sql $(SCORING) sql/CreateTables.sql
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f $(SHAREDIR)/extension/url.sql
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f $(SCORING)
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f sql/CreateTables.sql 
bin: 
	mkdir bin
bin/Anelosimus.Eximius : bin ecpg/Anelosimus.Eximius.pgc 
	cd ecpg
	ecpg -t -I/usr/include/postgresql -I/usr/include/curl ecpg/Anelosimus.Eximius.pgc
	gcc -g -DMAXCO=$(MAXCO) -D___FETCH___="$(FETCHQUERY)" -o bin/Anelosimus.Eximius -I /usr/include/postgresql/ ecpg/Anelosimus.Eximius.c -lecpg -lpq -lcurl
initialise:	$(INITIALURLS)
		sed "s:^:INSERT INTO node (url, score, depth) VALUES(':" $(INITIALURLS) | sed "s:$$:',1,0);:" > ./sql/initial.sql
		psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f ./sql/initial.sql
run:	./bin/Anelosimus.Eximius
	./bin/Anelosimus.Eximius $(USER) $(DBNAME)@$(HOST):$(PORT) 



