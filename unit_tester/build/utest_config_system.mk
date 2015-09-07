#
#
#

ifeq ($(PRODUCTION_ROOT),)
PRODUCTION_ROOT := $(realpath $(CURDIR)/../../)
ifneq ($(SILENCE), @)
$(info PRODUCTION_ROOT is not defined, using default [$(PRODUCTION_ROOT)].)
endif
endif

ifeq ($(CPPUTEST_HOME),)
$(error CPPUTEST_HOME is not defined! (add it to the system variables))
endif

ifeq ($(ARCH),)
$(error ARCH is not defined! Aborting...)
endif

COMPONENT_NAME 		:= $(APP_EXEC_NAME)_$(ARCH)

WORKSPACE_ROOT		:= $(realpath $(PRODUCTION_ROOT)/)
PRODUCTION_SOURCES	:= $(PRODUCTION_ROOT)/src
TEST_ROOT		:= $(PRODUCTION_ROOT)/unit_tester
TEST_SRC		:= $(TEST_ROOT)/tests

TESTS_PATH := $(realpath $(TEST_ROOT)/tests)

ifneq ($(SILENCE), @)
$(info *** ARCH=$(ARCH) ***)
$(info *** WORKSPACE_ROOT=$(WORKSPACE_ROOT) ***)
$(info *** CPPUTEST_HOME=$(CPPUTEST_HOME) ***)
$(info *** TEST_ROOT=$(TEST_ROOT) ***)
$(info *** TESTS_PATH=$(TESTS_PATH) ***)
endif

UNAME_OUTPUT := $(shell uname -a)
MINGW_STR := MINGW
CYGWIN_STR := CYGWIN
UNKNWOWN_OS_STR := Unknown
UNAME_OS := $(UNKNWOWN_OS_STR)

ifeq ($(findstring $(MINGW_STR),$(UNAME_OUTPUT)),$(MINGW_STR))
	UNAME_OS = $(MINGW_STR)
endif

ifeq ($(findstring $(CYGWIN_STR),$(UNAME_OUTPUT)),$(CYGWIN_STR))
	UNAME_OS = $(CYGWIN_STR)
	CPPUTEST_HOME := $(shell cygpath -u "${CPPUTEST_HOME}")
endif

CPPUTEST_CPPFLAGS += -DUT_ARCH_$(ARCH)
CPPUTEST_CPPFLAGS += -DUT_TESTS_PATH=\"$(TESTS_PATH)\"
