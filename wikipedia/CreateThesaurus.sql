CREATE EXTENSION dict_thesaurus32bits;

DROP TEXT SEARCH DICTIONARY IF EXISTS french_stemnostop; 
CREATE TEXT SEARCH DICTIONARY french_stemnostop (
    TEMPLATE = snowball,
    Language = french
);
DROP TEXT SEARCH DICTIONARY IF EXISTS wikipedia;
CREATE TEXT SEARCH DICTIONARY wikipedia (
    TEMPLATE = thesaurus_template32,
    DictFile = wikipedia,
    Dictionary = french_stemnostop
 );
ALTER TEXT SEARCH CONFIGURATION french
ALTER MAPPING FOR asciiword, asciihword, hword_asciipart, word WITH wikipedia, french_stemnostop;
