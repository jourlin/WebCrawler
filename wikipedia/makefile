include ~/.anelosimus/webcrawler.cfg 

all : migrate2PG 
frwiki-latest-page.sql.gz :
	wget http://dumps.wikimedia.org/frwiki/latest/frwiki-latest-page.sql.gz
frwiki-latest-page.sql : frwiki-latest-page.sql.gz
	gunzip frwiki-latest-page.sql.gz
wikipedia.page.sql : frwiki-latest-page.sql
	grep INSERT frwiki-latest-page.sql | sed 's:`:":g' | sed 's:\\\(.\):\1\1:g'> wikipedia.page.sql
frwiki-latest-redirect.sql.gz :
	wget http://dumps.wikimedia.org/frwiki/latest/frwiki-latest-redirect.sql.gz
frwiki-latest-redirect.sql : frwiki-latest-redirect.sql.gz
	gunzip frwiki-latest-redirect.sql.gz
wikipedia.redirect.sql : frwiki-latest-redirect.sql
	grep INSERT frwiki-latest-redirect.sql | sed 's:`:":g' | sed 's:\\\(.\):\1\1:g'> wikipedia.redirect.sql
migrate2PG:	wikipedia.page.sql wikipedia.redirect.sql CreateTables.sql
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "CreateTables.sql"
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "wikipedia.page.sql"
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "wikipedia.redirect.sql"
wikipedia.ths:	wikipedia.page.sql wikipedia.redirect.sql CreateTables.sql
	psql -t -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -c "SELECT lower(translate(page_title,'_',' '))||' : ['||page_title||']' FROM page WHERE page_is_redirect='0';"|sed 's:^ ::' | sed 's:_(.*)$:::'|sed 's: (.*) \:: \::'> wikipedia.ths
	psql -t -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -c "SELECT lower(translate(page_title,'_',' '))||' : ['||rd_title||']' FROM redirect,page WHERE rd_from=page_id;" | sed 's:^ ::'| sed 's:_(.*)$:::'|sed 's: (.*) \:: \::'>> wikipedia.ths
thesaurus:	wikipedia.ths CreateThesaurus.sql
	sudo cp wikipedia.ths /usr/local/pgsql/share/tsearch_data/
	psql -U$(USER) -h$(HOST) -p$(PORT) $(DBNAME) -f "CreateThesaurus.sql"
