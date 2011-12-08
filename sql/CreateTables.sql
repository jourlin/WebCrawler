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
CREATE VIEW linksview AS SELECT links.from as from, url as to, midcontext FROM links, nodeview WHERE links.to = nodeview.id;

CREATE OR REPLACE FUNCTION linksview_insert_row() RETURNS TRIGGER AS $linksview$
DECLARE
to_id BIGINT;
BEGIN
SELECT INTO to_id "id" FROM "node" WHERE url_out("url")=NEW."to";
        IF to_id IS NULL THEN
         INSERT INTO node (url) VALUES (url_in(NEW."to")) INTO to_id RETURNING id;
        END IF;
INSERT INTO links ("from", "to", midcontext, checked) VALUES (NEW."from", to_id, NEW.midcontext, now());
RETURN NEW;
        RETURN NULL; -- result is ignored since this is an AFTER trigger
END;
$linksview$ LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS ScoreURL(nodeid bigint);
CREATE FUNCTION ScoreURL(nodeid bigint) RETURNS bigint AS 
$$
DECLARE
score bigint;
tmp bigint;
BEGIN
select (CASE WHEN (substring(lower(url_out("url")), 'presidentielle') IS NULL) THEN 0 ELSE 2 END)+
(CASE WHEN (substring(lower(url_out("url")), 'politique') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower(url_out("url")), 'election') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower(url_out("url")), 'primaires') IS NULL) THEN 0 ELSE 3 END)+
(CASE WHEN url_top("url")='fr' THEN 10 ELSE 0 END)+
(CASE WHEN (substring(lower(url_out("url")), 'socialistes') IS NULL) THEN 0 ELSE 3 END)
INTO score FROM node where node."id"=nodeid;
RETURN score;
END;
$$ LANGUAGE plpgsql;

CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

