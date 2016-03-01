CC ?= gcc
CFLAGS_common ?= -Wall -std=gnu99 -DDEBUG
CFLAGS_orig = -O0
CFLAGS_opt  = -O0 -g
CFLAGS_opt_hash1  = -O0 -g -DHASH_1 -DDEBUG1
CFLAGS_opt_hash2  = -O0 -g -DHASH_2 -DDEBUG1

SRC_HASH = phonebook_opt_hash

EXEC = phonebook_orig phonebook_opt phonebook_opt_hash1 phonebook_opt_hash2
all: $(EXEC)

SRCS_common = main.c

phonebook_orig: $(SRCS_common) phonebook_orig.c phonebook_orig.h
	$(CC) $(CFLAGS_common) $(CFLAGS_orig) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

phonebook_opt: $(SRCS_common) phonebook_opt.c phonebook_opt.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

phonebook_opt_hash1: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_hash1) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c
phonebook_opt_hash2: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_hash2) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

run: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_orig && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run1: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run2: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash1 && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run3: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash2 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

cc:
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches

cache-test: $(EXEC)
	@rm -f orig.txt opt.txt opt_hash.txt
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_orig 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_hash1 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_hash2 1>/dev/null

report: $(EXEC)
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -g \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.orig ./phonebook_orig
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -g \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt ./phonebook_opt
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -g \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash1 ./phonebook_opt_hash1
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -g \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash2 ./phonebook_opt_hash2

test:
	$(CC) -pthread $(CFLAGS_common) $(CFLAGS_orig) \
	-o $@ $@.c
test1:
	$(CC) -pthread $(CFLAGS_common) $(CFLAGS_orig) \
	-o $@ $@.c

output.txt: cache-test calculate
	./calculate

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o perf.* \
		calculate orig.txt opt.txt opt_hash.txt output.txt runtime.png test test1
