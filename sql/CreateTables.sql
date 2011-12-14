CREATE LANGUAGE plpgsql;

DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url url NOT NULL UNIQUE,
    effectiveurl url,
    checked timestamp with time zone,
    score double precision,
    content text
);

-- importation : INSERT INTO node (url) SELECT CAST(url||urlpath AS url) FROM (SELECT DISTINCT url, urlpath FROM node_old WHERE length(url||urlpath) <1024) as x;

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

DROP VIEW IF EXISTS linksview;
CREATE VIEW linksview AS SELECT links.from as from, url as to, midcontext FROM links, node WHERE links.to = node.id;

CREATE OR REPLACE FUNCTION linksview_insert_row() RETURNS TRIGGER AS $linksview$
DECLARE
to_id BIGINT;
link_id BIGINT;
BEGIN
SELECT INTO to_id "id" FROM "node" WHERE "url"=NEW."to";
        IF to_id IS NULL THEN
         INSERT INTO node (url, score) VALUES (NEW."to", ScoreURL(NEW."to")) INTO to_id RETURNING id;
        END IF;
SELECT INTO link_id "id" FROM "links" WHERE "from"=NEW."from" AND "to"=to_id;
IF link_id IS NULL THEN
    INSERT INTO links ("from", "to", midcontext, checked) VALUES (NEW."from", to_id, NEW.midcontext, now());
END IF;
RETURN NEW;
        RETURN NULL; -- result is ignored since this is an AFTER trigger
END;


$linksview$ LANGUAGE plpgsql;

CREATE TRIGGER linksview_insert
    INSTEAD OF INSERT ON linksview
    FOR EACH ROW
    EXECUTE PROCEDURE linksview_insert_row();

CREATE OR REPLACE FUNCTION ScoreURL(url url) RETURNS bigint AS 
$$
DECLARE
score INT;
BEGIN
IF CAST(url_top(url) AS TEXT) ='fr' THEN
	score=1;
ELSE
	score=0;
END IF;
IF substring(CAST(url_sub(url) AS TEXT), 'presidentielles') IS NOT NULL THEN
	score+=2;
END IF;
IF substring(CAST(url_dom(url) AS TEXT), 'presidentielles') IS NOT NULL THEN
	score+=3;
END IF;
RETURN score;
END;
$$ LANGUAGE plpgsql;

CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

