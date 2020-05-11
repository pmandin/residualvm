MODULE := engines/reevengi

MODULE_OBJS := \
	detection.o \
	reevengi.o \
	formats/pak.o \
	formats/tim.o \
	re1/re1.o \
	re2/re2.o \
	re3/re3.o

# This module can be built as a plugin
ifeq ($(ENABLE_REEVENGI), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
