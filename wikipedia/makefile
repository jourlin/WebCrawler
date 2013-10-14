include ~/.anelosimus/webcrawler.cfg 
LANG = es
DBNAME = eswikipedia
all:	migrate2PG thesaurus wikifier_server wikifyer
clean:	
	rm *-latest-* *.ths wikipedia.* *~ wikifyer_server wikifyer_client
$(LANG)wiki-latest-page.sql.gz :
	wget http://dumps.wikimedia.org/$(LANG)wiki/latest/$(LANG)wiki-latest-page.sql.gz
$(LANG)wiki-latest-page.sql : $(LANG)wiki-latest-page.sql.gz
	gunzip $(LANG)wiki-latest-page.sql.gz
wikipedia.page.sql : $(LANG)wiki-latest-page.sql
	grep INSERT $(LANG)wiki-latest-page.sql | sed 's:`:":g' | sed 's:\\\(.\):\1\1:g'> wikipedia.page.sql
$(LANG)wiki-latest-redirect.sql.gz :
	wget http://dumps.wikimedia.org/$(LANG)wiki/latest/$(LANG)wiki-latest-redirect.sql.gz
$(LANG)wiki-latest-redirect.sql : $(LANG)wiki-latest-redirect.sql.gz
	gunzip $(LANG)wiki-latest-redirect.sql.gz
wikipedia.redirect.sql : $(LANG)wiki-latest-redirect.sql
	grep INSERT $(LANG)wiki-latest-redirect.sql | sed 's:`:":g' | sed 's:\\\(.\):\1\1:g'> wikipedia.redirect.sql
migrate2PG:	wikipedia.page.sql wikipedia.redirect.sql CreateTables.sql
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "CreateTables.sql"
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "wikipedia.page.sql"
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "wikipedia.redirect.sql"
wikipedia.ths: 
	psql -t -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -c "SELECT lower(translate(page_title,'_',' '))||' : ['||page_title||']' FROM page WHERE page_is_redirect='0';"|sed 's:^ ::' | sed 's:_(.*)$:::'|sed 's: (.*) \:: \::'> wikipedia.ths
	psql -t -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -c "SELECT lower(translate(page_title,'_',' '))||' : ['||rd_title||']' FROM redirect,page WHERE rd_from=page_id;" | sed 's:^ ::'| sed 's:_(.*)$:::'|sed 's: (.*) \:: \::'>> wikipedia.ths
thesaurus:	wikipedia.ths CreateThesaurus.sql $(LIBDIR)/dict_thesaurus32bits.so $(SHAREDIR)/extension/dict_thesaurus32bits.control
	sudo cp wikipedia.ths $(SHAREDIR)/tsearch_data/
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "CreateThesaurus.sql"
wikipedia.dic:	wikipedia.ths
		cat wikipedia.ths | sed 's/ \:.*//' > wikipedia.dic
wikifyer_server:	wikifyer_server.c 
	gcc -o wikifyer_server wikifyer_server.c
wikifyer_client:	wikifyer_client.c
	gcc -o wikifyer_client wikifyer_client.c
wikifyer:	wikifyer_server wikifyer_client
	

