SHELL = /bin/sh

#
# The Unit Test core build makefile.
# Performs the build in a non-recursive manner.
#

ifndef SILENCE
	SILENCE = @
#	SILENCE =
endif

APP_EXEC_NAME = app_tester

#######################################
# MK files to be included, *ordered*:

# Set system level build configuration.
MK_FILES +=	utest_config_system.mk

# Set test and production sources for the build.
MK_FILES +=	mk_$(ARCH)/utest_config_test_path.mk
MK_FILES +=	mk_$(ARCH)/utest_config_production_path.mk

# Set build options.
MK_FILES +=	utest_config_options.mk

# Apply configuration settings into command variables instructions.
MK_FILES +=	utest_apply_config.mk

include $(MK_FILES)



#Helper Functions
#~~~~~~~~~~~~~~~~~
#Returns all source files from a given directory (.c and .cpp)
get_src_from_dir  = $(wildcard $1/*.cpp) $(wildcard $1/*.c)

#Returns all source files from a given directory list.
get_src_from_dir_list = $(foreach dir, $1, $(call get_src_from_dir,$(dir)))

__src_to = $(subst .c,$1, $(subst .cpp,$1,$(if $(CPPUTEST_USE_VPATH),$(notdir $2),$2)))
src_to = $(addprefix $(CPPUTEST_OBJS_DIR)/,$(call __src_to,$1,$2))
src_to_o = $(call src_to,.o,$1)
src_to_d = $(call src_to,.d,$1)
src_to_gcda = $(call src_to,.gcda,$1)
src_to_gcno = $(call src_to,.gcno,$1)
time = $(shell date +%s)
debug_print_list = $(foreach word,$1,echo "  $(word)";) echo;




#Derived
STUFF_TO_CLEAN += $(TEST_TARGET) $(TEST_TARGET).exe $(TARGET_LIB) $(TARGET_MAP)

SRC += $(call get_src_from_dir_list, $(SRC_DIRS)) $(SRC_FILES)			        
OBJ = $(call src_to_o,$(SRC))

STUFF_TO_CLEAN += $(OBJ)

TEST_SRC = $(call get_src_from_dir_list, $(TEST_SRC_DIRS))
TEST_OBJS = $(call src_to_o,$(TEST_SRC))
STUFF_TO_CLEAN += $(TEST_OBJS)


MOCKS_SRC = $(call get_src_from_dir_list, $(MOCKS_SRC_DIRS))
MOCKS_OBJS = $(call src_to_o,$(MOCKS_SRC))
STUFF_TO_CLEAN += $(MOCKS_OBJS)

ALL_SRC = $(SRC) $(TEST_SRC) $(MOCKS_SRC)

# If we're using VPATH
ifeq ($(CPPUTEST_USE_VPATH), Y)
	# gather all the source directories and add them
	VPATH += $(sort $(dir $(ALL_SRC)))
	# Add the component name to the objs dir path, to differentiate between same-name objects
	CPPUTEST_OBJS_DIR := $(addsuffix /$(COMPONENT_NAME),$(CPPUTEST_OBJS_DIR))
endif

#Test coverage with gcov
GCOV_OUTPUT = gcov_output.txt
GCOV_REPORT = gcov_report.txt
GCOV_ERROR = gcov_error.txt
GCOV_GCDA_FILES = $(call src_to_gcda, $(ALL_SRC))
GCOV_GCNO_FILES = $(call src_to_gcno, $(ALL_SRC))
TEST_OUTPUT = $(TEST_TARGET).txt
STUFF_TO_CLEAN += \
	$(GCOV_OUTPUT)\
	$(GCOV_REPORT)\
	$(GCOV_REPORT).html\
	$(GCOV_ERROR)\
	$(GCOV_GCDA_FILES)\
	$(GCOV_GCNO_FILES)\
	$(TEST_OUTPUT)


CPPUTEST_CPPFLAGS +=  $(INCLUDES) 

#The gcda files for gcov need to be deleted before each run
#To avoid annoying messages.
GCOV_CLEAN = $(SILENCE)rm -f $(GCOV_GCDA_FILES) $(GCOV_OUTPUT) $(GCOV_REPORT) $(GCOV_ERROR)
RUN_TEST_TARGET = $(SILENCE)  $(GCOV_CLEAN) ; echo "Running $(TEST_TARGET)"; ./$(TEST_TARGET) $(CPPUTEST_EXE_FLAGS)

# get_dirs_from_dirspec  = $(wildcard $1)
# INCLUDES_DIRS_EXPANDED = $(call get_dirs_from_dirspec, $(INCLUDE_DIRS))
INCLUDES += $(foreach dir, $(INCLUDE_DIRS), -I$(dir))
#MOCK_DIRS_EXPANDED = $(call get_dirs_from_dirspec, $(MOCKS_SRC_DIRS))
#INCLUDES += $(foreach dir, $(MOCKS_SRC_DIRS), -I$(dir))


DEP_FILES = $(call src_to_d, $(ALL_SRC))
STUFF_TO_CLEAN += $(DEP_FILES) $(PRODUCTION_CODE_START) $(PRODUCTION_CODE_END)
STUFF_TO_CLEAN += $(STDLIB_CODE_START) $(MAP_FILE)

# We'll use the CPPUTEST_CFLAGS etc so that you can override AND add to the CppUTest flags
CFLAGS = $(CPPUTEST_CFLAGS) $(CPPUTEST_ADDITIONAL_CFLAGS)
CPPFLAGS = $(CPPUTEST_CPPFLAGS) $(CPPUTEST_ADDITIONAL_CPPFLAGS)
CXXFLAGS = $(CPPUTEST_CXXFLAGS) $(CPPUTEST_ADDITIONAL_CXXFLAGS)
LDFLAGS = $(CPPUTEST_LDFLAGS) $(CPPUTEST_ADDITIONAL_LDFLAGS)
	
	
# Targets

.PHONY: all
all: start $(TEST_TARGET)  
	$(RUN_TEST_TARGET)	

.PHONY: start
start: $(TEST_TARGET) 
	$(SILENCE)START_TIME=$(call time)

# Report all flags set.
.PHONY: flags
flags: 
	@echo
	@echo "OS ${UNAME_OS} from ${UNAME_OUTPUT}"
	@echo "Compile C and C++ source with CPPFLAGS:"
	@$(call debug_print_list,$(CPPFLAGS))
	@echo "Compile C++ source with CXXFLAGS:"
	@$(call debug_print_list,$(CXXFLAGS))
	@echo "Compile C source with CFLAGS:"
	@$(call debug_print_list,$(CFLAGS))
	@echo "Link with LDFLAGS:"
	@$(call debug_print_list,$(LDFLAGS))
	@echo "Link with LD_LIBRARIES:"
	@$(call debug_print_list,$(LD_LIBRARIES))
	@echo "Create libraries with ARFLAGS:"
	@$(call debug_print_list,$(ARFLAGS))	
	
	
$(TEST_TARGET): $(TEST_OBJS) $(MOCKS_OBJS)  $(PRODUCTION_CODE_START) $(TARGET_LIB) $(USER_LIBS) $(PRODUCTION_CODE_END) $(CPPUTEST_LIB) $(STDLIB_CODE_START) 
	$(SILENCE)echo Linking $@
	$(SILENCE)$(LINK.cpp) -o $@ -Wl,--whole-archive $^ -Wl,--no-whole-archive $(LD_LIBRARIES)

$(TARGET_LIB): $(OBJ)
	$(SILENCE)echo Building archive $@
	$(SILENCE)mkdir -p $(CPPUTEST_LIB_DIR)
	$(SILENCE)$(AR) $(ARFLAGS) $@ $^
#	$(SILENCE)$(RANLIB) $@

$(CPPUTEST_OBJS_DIR)/%.o: %.cpp
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.cpp) -MMD -MP $(OUTPUT_OPTION) $<

$(CPPUTEST_OBJS_DIR)/%.o: %.c
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.c) -MMD -MP  $(OUTPUT_OPTION) $<

#??????????????
-include $(DEP_FILES)	
	
.PHONY: clean
clean:
	$(SILENCE)echo Making clean
	$(SILENCE)$(RM) $(STUFF_TO_CLEAN)
	$(SILENCE)rm -rf gcov $(CPPUTEST_OBJS_DIR) $(CPPUTEST_LIB_DIR)
	$(SILENCE)find . -name "*.gcno" | xargs rm -f
	$(SILENCE)find . -name "*.gcda" | xargs rm -f
	
#realclean gets rid of all gcov, o and d files in the directory tree
#not just the ones made by this makefile
.PHONY: realclean
realclean: clean
	$(SILENCE)rm -rf gcov
	$(SILENCE)find . -name "*.gdcno" | xargs rm -f
	$(SILENCE)find . -name "*.[do]" | xargs rm -f	

gcov: test
ifeq ($(CPPUTEST_USE_VPATH), Y)
	$(SILENCE)gcov --object-directory $(CPPUTEST_OBJS_DIR) $(SRC) >> $(GCOV_OUTPUT) 2>> $(GCOV_ERROR)
else
	$(SILENCE)for d in $(SRC_DIRS) ; do \
		gcov --object-directory $(CPPUTEST_OBJS_DIR)/$$d $$d/*.c $$d/*.cpp >> $(GCOV_OUTPUT) 2>>$(GCOV_ERROR) ; \
	done
	$(SILENCE)for f in $(SRC_FILES) ; do \
		gcov --object-directory $(CPPUTEST_OBJS_DIR)/$$f $$f >> $(GCOV_OUTPUT) 2>>$(GCOV_ERROR) ; \
	done
endif
	$(CPPUTEST_HOME)/scripts/filterGcov.sh $(GCOV_OUTPUT) $(GCOV_ERROR) $(GCOV_REPORT) $(TEST_OUTPUT)
	$(SILENCE)cat $(GCOV_REPORT)
	$(SILENCE)mkdir -p gcov
	$(SILENCE)mv *.gcov gcov
	$(SILENCE)mv gcov_* gcov
	$(SILENCE)echo "See gcov directory for details"
 
debug:
	@echo
	@echo "Target Source files:"
	@$(call debug_print_list,$(SRC))
	@echo "Target Object files:"
	@$(call debug_print_list,$(OBJ))
	@echo "Test Source files:"
	@$(call debug_print_list,$(TEST_SRC))
	@echo "Test Object files:"
	@$(call debug_print_list,$(TEST_OBJS))
	@echo "Mock Source files:"
	@$(call debug_print_list,$(MOCKS_SRC))
	@echo "Mock Object files:"
	@$(call debug_print_list,$(MOCKS_OBJS))
	@echo "All Input Dependency files:"
	@$(call debug_print_list,$(DEP_FILES))
	@echo Stuff to clean:
	@$(call debug_print_list,$(STUFF_TO_CLEAN))
	@echo Includes:
	@$(call debug_print_list,$(INCLUDES)) 
	
