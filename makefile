USER = eric 
DBNAME = campagne-day-`date +"%Y-%m-%d"`
HOST = localhost
PORT = 5435
PASSWORD= xxxxxx 

TARGET =$(DBNAME)@$(HOST):$(PORT)

# Usual path for pg_config :
# PG_CONFIG = pg_config
# Or specific :
 PG_CONFIG = /usr/local/pgsql/bin/pg_config
SHAREDIR = $(shell $(PG_CONFIG) --sharedir)

all:  bin/Anelosimus.Eximius.daily
clean: 
	rm bin/Anelosimus.Eximius.daily
	rm $(SHAREDIR)/extension/url.*

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

CreateTables:  sql/CreateTables.sql 
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f $(SHAREDIR)/extension/url.sql
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f sql/CreateTables.sql 
bin: 
	mkdir bin
bin/Anelosimus.Eximius.daily : bin ecpg/Anelosimus.Eximius.pgc 
	cd ecpg
	ecpg -t -I/usr/include/postgresql -I/usr/include/curl ecpg/Anelosimus.Eximius.pgc
	gcc -g -D_TARGET_=\"$(TARGET)\" -D_USER_=\"$(USER)\" -D_PASSWORD_=\"$(PASSWORD)\" -o bin/Anelosimus.Eximius.daily -I /usr/include/postgresql/ ecpg/Anelosimus.Eximius.c -lecpg -lpq -lcurl

init:	
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME) -c "INSERT INTO node (url, score, depth) VALUES ('http://dev.termwatch.es/~jourlin/campagne/sitesdescandidats.html', 1, 0)"
	psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME) -c "INSERT INTO node (url, score, depth) VALUES ('http://dev.termwatch.es/~jourlin/campagne/pagespolitiques.html', 1, 0)"

launch: bin/Anelosimus.Eximius.daily
	./bin/Anelosimus.Eximius.daily &
	sleep 10
	./bin/Anelosimus.Eximius.daily &
	sleep 10
	./bin/Anelosimus.Eximius.daily &
dropnewday: 
	dropdb -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME)
createnewday: 
	createdb -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME)

	

