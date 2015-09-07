#
#
#

# Make empty, instead of 'N', for usage in $(if ) conditionals
ifneq ($(CPPUTEST_USE_VPATH), Y)
	CPPUTEST_USE_VPATH := 
endif

ifeq ($(CPPUTEST_ENABLE_DEBUG), Y)
	CPPUTEST_CPPFLAGS += -g
endif

ifeq ($(CPPUTEST_ENABLE_C99), Y)
	CPPUTEST_CFLAGS += -std=c99
endif

ifeq ($(CPPUTEST_ENABLE_C++11), Y)
	CPPUTEST_CXXFLAGS += -std=c++11
endif

CPPUTEST_CXXFLAGS += -include $(TEST_ROOT)/mocks/include/oper_new_mock.h

ifeq ($(UNAME_OS),  $(MINGW_STR))
	CPPUTEST_CPPFLAGS += -include $(CURDIR)/mingw_prepand.h
endif


# When CPPUTEST_HOME is NOT defined, 
# it is assumed that the framework has been installed system wide.
# When defined, the paths to the libs need to be set.
ifneq ($(CPPUTEST_HOME),)
	LD_LIBRARIES += -L$(CPPUTEST_HOME)/lib
endif


CPPUTEST_CPPFLAGS += -I$(CPPUTEST_HOME)/include/CppUTestExt/CppUTestGMock
CPPUTEST_CPPFLAGS += -I$(CPPUTEST_HOME)/include/CppUTestExt/CppUTestGTest

ifeq ($(CPPUTEST_USE_GCOV), Y)
	CPPUTEST_CPPFLAGS += -fprofile-arcs -ftest-coverage
endif

ifeq ($(CPPUTEST_MEMORY_LEAK_DETECTION), Y)
     CPPUTEST_CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
     CPPUTEST_CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
endif

CPPUTEST_CPPFLAGS += $(CPPUTEST_WARNINGFLAGS)
CPPUTEST_CXXFLAGS += $(CPPUTEST_CXX_WARNINGFLAGS)

TARGET_MAP = $(COMPONENT_NAME).map.txt
ifeq ($(CPPUTEST_MAP_FILE), Y)
	CPPUTEST_LDFLAGS += -Wl,-map,$(TARGET_MAP)
endif


LD_LIBRARIES += -lCppUTest

#LD_LIBRARIES += -l<my_library>
#LD_LIBRARIES += -u <my_constructor>

ifeq ($(CPPUTEST_USE_EXTENSIONS), Y)
	LD_LIBRARIES += -lCppUTestExt
endif

LD_LIBRARIES += -lstdc++
ifeq ($(CPPUTEST_USE_GCOV), Y)
	LD_LIBRARIES += -lgcov
endif

TARGET_LIB = \
    $(CPPUTEST_LIB_DIR)/lib$(COMPONENT_NAME).a
    
TEST_TARGET = \
    $(COMPONENT_NAME)
