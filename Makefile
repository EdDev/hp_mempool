
#HDR += 
#SRC += 
#OBJ = $(SRC:.c=.o)

EXEC = objmempool_example

.PHONY: all
all: utest

.PHONY: utest
utest:
	$(MAKE) -C unit_tester

#$(EXEC):$(OBJ)
#	$(CC) -Wall -Werror -O0 -g -o $(EXEC) *.o

#%.o: %.c $(HDR) 
#	$(CC) -Wall -Werror -O0 -g -c -I./src $<

clean:
#	rm -f $(EXEC)
#	rm -f *.o
	$(MAKE) -C unit_tester $@
