
$(info *******************************************)
$(info ************ UNIT TESTER BUILD ************)
$(info *******************************************)

ifndef SILENCE
	SILENCE = @
#	SILENCE =
endif

.PHONY: all
all: common

.PHONY: common
common:
	$(MAKE) -C ./build ARCH=common

.PHONY: clean
clean:
	$(MAKE) -C ./build	$@ ARCH=common