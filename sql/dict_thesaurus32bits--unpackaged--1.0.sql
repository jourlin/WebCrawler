/* contrib/dict_xsyn/dict_thesaurus32bits--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION dict_thesaurus32bits" to load this file. \quit

ALTER EXTENSION dict_thesaurus32bits ADD function thesaurus_init(internal);
ALTER EXTENSION dict_thesaurus32bits ADD function thesaurus_lexize(internal,internal,internal,internal);
ALTER EXTENSION dict_thesaurus32bits ADD text search template thesaurus_template;
ALTER EXTENSION dict_thesaurus32bits ADD text search dictionary thesaurus;
