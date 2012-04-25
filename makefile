# If ~/.anelosimus does not exists
# You might want to copy the sample config by executing :
# mkdir ~/.anelosimus; cp cfg/* ~/.anelosimus

include ~/.anelosimus/webcrawler.cfg 

all:  bin/Anelosimus.Eximius
clean:  DropDataBase
	rm bin/Anelosimus.Eximius
	rm $(SHAREDIR)/extension/url.*
DropDataBase:
	dropdb -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) 

extensions: $(SHAREDIR)/extension/url.sql  $(SHAREDIR)/extension/url.so $(SHAREDIR)/extension/dict_thesaurus32bits.control $(SHAREDIR)/extension/dict_thesaurus32bits--1.0.sql $(LIBDIR)/dict_thesaurus32bits.so

$(SHAREDIR)/extension/dict_thesaurus32bits.control:	sql/dict_thesaurus32bits.control
	sudo cp sql/dict_thesaurus32bits.control $(SHAREDIR)/extension/
$(SHAREDIR)/extension/dict_thesaurus32bits--1.0.sql:	sql/dict_thesaurus32bits--1.0.sql
	sudo cp sql/dict_thesaurus32bits--1.0.sql $(SHAREDIR)/extension/
$(LIBDIR)/dict_thesaurus32bits.so:	c/dict_thesaurus32bits.so
	sudo cp c/dict_thesaurus32bits.so $(LIBDIR)/
$(SHAREDIR)/extension/url.so:	c/url.so
	sudo cp c/url.so $(SHAREDIR)/extension/url.so
	sudo chown postgres.postgres $(SHAREDIR)/extension/url.so
$(SHAREDIR)/extension/url.sql:	
	sed "s:_OBJWD_:"$(SHAREDIR)"/extension:g" sql/url.sql > $(SHAREDIR)/extension/url.sql
	chown postgres.postgres $(SHAREDIR)/extension/url.sql
c/url.so: c/url.c
	cd c; make ; cd .. 
c/dict_thesaurus32bits.so:	c/dict_thesaurus32bits.c
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
	ecpg -t -I/usr/include/postgresql ecpg/Anelosimus.Eximius.pgc
	gcc -D___FETCH___="$(FETCHQUERY)" -o bin/Anelosimus.Eximius -I /usr/include/postgresql/ ecpg/Anelosimus.Eximius.c -lecpg -lpq 
initialise:	$(INITIALURLS)
		sed "s:^:INSERT INTO node (url, score, depth) VALUES(':" $(INITIALURLS) | sed "s:$$:',1,0);:" > ./sql/initial.sql
		psql -U$(USER) -h$(HOST) -p$(PORT) -d$(DBNAME)  -f ./sql/initial.sql
run:	./bin/Anelosimus.Eximius
	./bin/Anelosimus.Eximius $(USER) $(DBNAME)@$(HOST):$(PORT) 



