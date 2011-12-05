#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for tutorial
#
# By default, this builds against an existing PostgreSQL installation
# (the one identified by whichever pg_config is first in your path).
# Within a configured source tree, you can say "gmake NO_PGXS=1 all"
# to build using the surrounding source tree.
#
# IDENTIFICATION
#    src/tutorial/Makefile
#
#-------------------------------------------------------------------------

MODULES = url 

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

