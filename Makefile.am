SUBDIRS = . tests


libjsonschema_c_la_CFLAGS = -I/usr/local/include/json-c
libjsonschema_c_la_LDFLAGS = -L/usr/local/lib -ljson-c

lib_LTLIBRARIES = libjsonschema-c.la
libjsonschema_c_la_SOURCES = 	schema_validator.c \
				instance_validator.c \
				utils.c
				