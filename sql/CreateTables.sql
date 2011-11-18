CREATE LANGUAGE plpgsql;

DROP TABLE IF EXISTS domain CASCADE;
CREATE TABLE domain (
    id BIGSERIAL PRIMARY KEY,
    checked timestamp with time zone,
    protocol VARCHAR(50) NOT NULL,
    subdomains VARCHAR(256),
    domain VARCHAR(256) NOT NULL,
    tld VARCHAR(10) NOT NULL,
    ipadr inet,
    UNIQUE(protocol, subdomains, domain, tld)
);

-- Note : When table "domain" is already filled, the two following queries insert root URLs into table node
--        This can be useful to start a crawl from previous collected data.
-- INSERT INTO node (url, score, domainid, urlpath) SELECT protocol||'://'||subdomains||'.'||domain||'.'||tld, 1.0, id, '' FROM domain WHERE subdomains is not NULL AND length(subdomains)!=0;
-- INSERT INTO node (url, score, domainid, urlpath) SELECT protocol||'://'||domain||'.'||tld, 1.0, id, '' FROM domain WHERE subdomains is NULL OR length(subdomains)=0;


DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url text NOT NULL,
    effectiveurl text,
    checked timestamp with time zone,
    score double precision,
    domainid bigint NOT NULL REFERENCES domain(id) ON UPDATE CASCADE ON DELETE CASCADE,
    urlpath text,
    content text,
    UNIQUE(url,urlpath)
);

DROP TABLE IF EXISTS links CASCADE;
CREATE TABLE links (
    id BIGSERIAL PRIMARY KEY,
    "from" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "to" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    leftcontext text,
    midcontext text,
    rightcontext text,
    UNIQUE ("from", "to"),
    checked timestamp with time zone
);

/* nodeview allows PostgreSQL to take the URL processing in charge */

DROP VIEW IF EXISTS nodeview;
CREATE VIEW nodeview AS select id, url||urlpath as url, effectiveurl, content FROM node;

CREATE OR REPLACE FUNCTION nodeview_insert_row() RETURNS TRIGGER AS $nodeview$
DECLARE
    domainid       BIGINT;
    nodeid           BIGINT;
    url                 VARCHAR[]; 
    domains        VARCHAR[];
    subdomains2 VARCHAR;
    domain2        VARCHAR;
    tld2               VARCHAR;
    url2               VARCHAR;
    urlpath2        VARCHAR;

    BEGIN
            SELECT INTO url  regexp_split_to_array(NEW.url, '[/:]+');
            SELECT INTO domains string_to_array(url[2], '.');
            tld2:= domains[array_upper(domains,1)];
            domain2:=domains[array_upper(domains,1)-1];
            IF (char_length(url[2]) = char_length(domain2) + char_length(tld2) + 1)
            THEN 
                    subdomains2:='';
            ELSE
                    subdomains2:=substr(url[2], 1, char_length(url[2])-char_length(domain2)-char_length(tld2)-2); 
            END IF;
            SELECT INTO domainid "id" FROM "domain" WHERE "protocol"=url[1] AND "subdomains"=subdomains2 AND "domain"=domain2 AND "tld"=tld2;
            IF domainid IS NULL THEN
                 INSERT INTO domain (protocol, subdomains, domain, tld) VALUES (url[1],subdomains2, domain2, tld2) INTO domainid RETURNING id;
            END IF;
            url2:=substr(NEW.url, 1, char_length(url[1])+3+char_length(url[2]));
            urlpath2:=substr(NEW.url, char_length(url2)+1, char_length(NEW.url)-char_length(url2));
            INSERT INTO node (url, urlpath, domainid, effectiveurl, content) VALUES (url2, urlpath2, domainid, NEW.effectiveurl, NEW.content) INTO nodeid RETURNING id;
	    UPDATE node SET score=ScoreURL(nodeid) WHERE id=nodeid;
            NEW.id=nodeid;
            RETURN NEW;
        RETURN NULL; -- result is ignored since this is an AFTER trigger
    END;
$nodeview$ LANGUAGE plpgsql;

CREATE TRIGGER nodeview_insert
    INSTEAD OF INSERT ON nodeview
    FOR EACH ROW
    EXECUTE PROCEDURE nodeview_insert_row();

DROP VIEW IF EXISTS linksview;
CREATE VIEW linksview AS SELECT links.from as from, url as to, midcontext FROM links, nodeview WHERE links.to = nodeview.id;

CREATE OR REPLACE FUNCTION linksview_insert_row() RETURNS TRIGGER AS $linksview$
DECLARE
	to_id		BIGINT;
BEGIN
	SELECT INTO to_id "id" FROM "nodeview" WHERE "url"=NEW."to";
        IF to_id IS NULL THEN
        	INSERT INTO nodeview (url) VALUES (NEW."to") INTO to_id RETURNING id;
        END IF;
	INSERT INTO links ("from", "to", midcontext, checked) VALUES (NEW."from", to_id, NEW.midcontext, now());
	RETURN NEW;
        RETURN NULL; -- result is ignored since this is an AFTER trigger
END;
$linksview$ LANGUAGE plpgsql;

CREATE TRIGGER linksview_insert
    INSTEAD OF INSERT ON linksview
    FOR EACH ROW
    EXECUTE PROCEDURE linksview_insert_row();


DROP FUNCTION IF EXISTS ScoreURL(nodeid bigint);
CREATE FUNCTION ScoreURL(nodeid bigint) RETURNS bigint AS 
$$
DECLARE
score bigint;
tmp bigint;

BEGIN
select count(*) INTO tmp FROM links where (
substring(lower(midcontext), 'presidentielle') is not NULL OR 
substring(lower(midcontext), 'politique') is not NULL OR 
substring(lower(midcontext), 'election') is not NULL OR 
substring(lower(midcontext), 'primaires') is not NULL OR 
substring(lower(midcontext), 'socialistes') is not NULL) AND
"to"=nodeid;
score:=tmp;
select (CASE WHEN (substring(lower("url"), 'presidentielle') IS NULL) THEN 0 ELSE 2 END)+
(CASE WHEN (substring(lower("url"), 'politique') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'election') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'primaires') IS NULL) THEN 0 ELSE 3 END)+
(CASE WHEN (substring(lower("url"), 'socialistes') IS NULL) THEN 0 ELSE 3 END)
INTO tmp FROM node where node."id"=nodeid;
score:=score+tmp;
SELECT case WHEN tld='fr' THEN 1 ELSE 0 END INTO tmp FROM node, domain WHERE node."id"=nodeid AND domainid=domain.id;
score:=score+tmp;
RETURN score;
END;
$$ LANGUAGE plpgsql;

CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

