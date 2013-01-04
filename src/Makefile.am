bin_PROGRAMS = superaes
lib_LTLIBRARIES = libsuperaes.la

superaes_SOURCES = main.c key.h convert.h
superaes_LDADD = libsuperaes.la

libsuperaes_la_SOURCES = superaes.c tables.c constants.h key.c superaes.h tables.h key.h _tables_common.h
