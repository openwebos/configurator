# Device makefile

BUILD_TYPE := release
PLATFORM := linux-arm
OBJDIR := $(BUILD_TYPE)-$(PLATFORM)
LOCAL_INCLUDES := -I$(QPEDIR)/include/mojodb
LOCAL_CFLAGS := $(CFLAGS) -Wall -Werror -DMOJ_LINUX $(LOCAL_INCLUDES) $(shell pkg-config --cflags glib-2.0)
LOCAL_CPPFLAGS := $(CPPFLAGS) -fno-rtti

include Makefile.inc