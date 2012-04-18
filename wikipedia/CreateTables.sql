DROP TABLE IF EXISTS "page"; 
CREATE TABLE "page" (
  "page_id" integer NOT NULL,
  "page_namespace" integer NOT NULL DEFAULT '0',
  "page_title" character varying(255) NOT NULL DEFAULT '',
  "page_restrictions" character varying(255) NOT NULL DEFAULT '',
  "page_counter" bigint  NOT NULL DEFAULT '0',
  "page_is_redirect" smallint  NOT NULL DEFAULT '0',
  "page_is_new" smallint  NOT NULL DEFAULT '0',
  "page_random" double precision NOT NULL DEFAULT '0',
  "page_touched" character varying(14) NOT NULL DEFAULT '',
  "page_latest" integer  NOT NULL DEFAULT '0',
  "page_len" integer  NOT NULL DEFAULT '0',
  "page_no_title_convert" smallint NOT NULL DEFAULT '0',
  PRIMARY KEY ("page_id")
) ;
DROP TABLE IF EXISTS "redirect"; 
CREATE TABLE "redirect" (
  "rd_from" integer NOT NULL DEFAULT '0',
  "rd_namespace" integer NOT NULL DEFAULT '0',
  "rd_title" character varying (255) NOT NULL DEFAULT '',
  "rd_interwiki" character varying (32) DEFAULT NULL,
  "rd_fragment" character varying (255) DEFAULT NULL,
  PRIMARY KEY ("rd_from")
) ;
