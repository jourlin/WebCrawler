CREATE LANGUAGE plpgsql;

DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url url NOT NULL UNIQUE,
    effectiveurl url,
    checked timestamp with time zone,
    score double precision,
    depth INTEGER,
    content text
);

CREATE INDEX node_ind ON node
   USING btree(url url_ops);

-- importation : INSERT INTO node (url) SELECT CAST(url||urlpath AS url) FROM (SELECT DISTINCT url, urlpath FROM node_old WHERE length(url||urlpath) <1024) as x;

DROP TABLE IF EXISTS links CASCADE;
CREATE TABLE links (
    id BIGSERIAL PRIMARY KEY,
    "from" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "to" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    count	INTEGER,
    leftcontext text,
    midcontext text,
    rightcontext text,
    UNIQUE ("from", "to"),
    checked timestamp with time zone
);

CREATE OR REPLACE FUNCTION normalize(str TEXT) RETURNS TEXT AS 
$$
DECLARE
BEGIN
RETURN lower(replace(replace(replace(replace(str, 'Ã©', 'e'), '&eacute;', 'e'), 'Ã¨', 'e'), '&egrave;', 'e'));
END;
$$ LANGUAGE plpgsql;

DROP VIEW IF EXISTS linksview;
CREATE VIEW linksview AS SELECT links.from as from, url as to, leftcontext, midcontext, rightcontext FROM links, node WHERE links.to = node.id;

CREATE OR REPLACE FUNCTION linksview_insert_row() RETURNS TRIGGER AS $linksview$
DECLARE
to_id BIGINT;
link_id BIGINT;
from_depth INTEGER;
old_depth INTEGER;
new_score INTEGER;
context   TEXT;
BEGIN
SELECT INTO from_depth "depth" FROM "node" WHERE "id"=NEW."from";
SELECT INTO to_id, old_depth "id","depth" FROM "node" WHERE "url"=NEW."to";
IF to_id IS NULL THEN
         INSERT INTO node (url, score, depth) VALUES (NEW."to", ScoreURL(NEW."to"), from_depth+1) INTO to_id RETURNING id;
ELSE
         IF from_depth+1 < old_depth THEN
             UPDATE node SET depth=from_depth+1 WHERE "id"=to_id;
         END IF;
END IF;
SELECT INTO link_id "id" FROM "links" WHERE "from"=NEW."from" AND "to"=to_id;
IF link_id IS NULL THEN
    INSERT INTO links ("from", "to", leftcontext, midcontext, rightcontext, checked, "count") VALUES (NEW."from", to_id, NEW.leftcontext, NEW.midcontext, NEW.rightcontext, now(), 1);
ELSE
    UPDATE links SET "count"="count"+1, leftcontext= leftcontext ||';'|| NEW.leftcontext, midcontext= midcontext ||';'|| NEW.midcontext, rightcontext= rightcontext ||';'|| NEW.rightcontext WHERE "from"=NEW."from" AND "to"=to_id;
END IF;

-- Calculate score relating to citation context

context ='';
IF NEW.leftcontext IS NOT NULL THEN 
    context = context || NEW.leftcontext;
END IF;
IF NEW.midcontext IS NOT NULL THEN 
    context = context || NEW.midcontext;
END IF;
IF NEW.rightcontext IS NOT NULL THEN 
    context = context || NEW.rightcontext;
END IF;

new_score = ScoreLink(context);

IF new_score > 0 THEN
    UPDATE node SET score=score+new_score WHERE url=NEW."to";
END IF;
RETURN NEW;
RETURN NULL; -- result is ignored since this is an AFTER trigger
END;
$linksview$ LANGUAGE plpgsql;

CREATE TRIGGER linksview_insert
    INSTEAD OF INSERT ON linksview
    FOR EACH ROW
    EXECUTE PROCEDURE linksview_insert_row();

CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

