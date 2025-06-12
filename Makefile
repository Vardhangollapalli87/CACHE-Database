
# flags =-O2 -Wall -std=c2x

# all: clean tree cache22

# tree : tree.o
# 	cc ${flags} $^ -o $@ ${ldflags}

# tree.o: tree.c
# 	cc ${flags} -c $^

# cache22: cache22.o
# 	cc ${flags} $^ -o $@ ${ldflags}

# cache22.o: cache22.c
# 	cc ${flags} -c $^

# clean:
# 	rm -f *.o tree cache22 



CC = cc
FLAGS = -O2 -Wall -std=c2x
LDFLAGS = 

all: clean cache  # Only need to build `cache22`

cache: cache.o tree.o
	$(CC) $(FLAGS) $^ -o $@ $(LDFLAGS)

tree.o: tree.c tree.h
	$(CC) $(FLAGS) -c $<

cache.o: cache.c tree.h
	$(CC) $(FLAGS) -c $<

clean:
	rm -f *.o cache
