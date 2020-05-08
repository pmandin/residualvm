MODULE := engines/reevengi

MODULE_OBJS := \
	detection.o \
	reevengi.o

# This module can be built as a plugin
ifeq ($(ENABLE_REEVENGI), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
