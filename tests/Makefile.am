TESTS=
TESTS+= check_jsonschema_c

check_PROGRAMS = 
check_PROGRAMS += $(TESTS:.test=)
check_PROGRAMS += check_jsonschema_c

check_jsonschema_c_SOURCES = schema_validator.h jsonschema-test.c
check_jsonschema_c_CFLAGS = -I../ -I/usr/local/include/json-c -O0
check_jsonschema_c_LDFLAGS = -L/usr/local/lib -L/usr/local/lib64
check_jsonschema_c_LDADD = $(top_builddir)/libjsonschema-c.la

EXTRA_DIST=
EXTRA_DIST += $(TESTS)


distclean-local:
	-rm -rf $(testsubdir)
