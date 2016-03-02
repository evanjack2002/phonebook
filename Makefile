CC ?= gcc
CFLAGS_common ?= -Wall -std=gnu99 -g -DDEBUG
CFLAGS_orig = -O0
CFLAGS_opt  = -O0
CFLAGS_opt_hash1  = -O0 -DHASH_1 -DDEBUG1
CFLAGS_opt_hash2  = -O0 -DHASH_2 -DDEBUG1
CFLAGS_opt_thread  = -O0 -pthread -DHASH_2 -DTHD -DDEBUG1

EXEC = phonebook_orig phonebook_opt phonebook_opt_hash1 phonebook_opt_hash2
all: $(EXEC)

SRCS_common = main.c

SRC_HASH = phonebook_opt_hash

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

thd: phonebook_opt_thread

phonebook_opt_thread: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_thread) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

run1: phonebook_orig
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_orig && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run2: phonebook_opt
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run3: phonebook_opt_hash1
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash1 && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run4: phonebook_opt_hash2
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash2 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

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

cc:
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	echo 1 | sudo tee /proc/sys/vm/drop_caches

rpt1: rpt_1
	perf report -i perf.orig
rpt2: rpt_2
	perf report -i perf.opt
rpt3: rpt_3
	perf report -i perf.opt_hash1
rpt4: rpt_4
	perf report -i perf.opt_hash2
rpt_1: phonebook_orig cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.orig ./phonebook_orig
rpt_2: phonebook_opt cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt ./phonebook_opt
rpt_3: phonebook_opt_hash1 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash1 ./phonebook_opt_hash1
rpt_4: phonebook_opt_hash2 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash2 ./phonebook_opt_hash2
rpt: rpt_1 rpt_2 rpt_3 rpt_4

test: test.c
	$(CC) -pthread $(CFLAGS_common) $(CFLAGS_orig) \
	-o $@ $@.c
test1: test1.c
	$(CC) -pthread $(CFLAGS_common) $(CFLAGS_orig) \
	-o $@ $@.c
test2: test2.c
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
		calculate orig.txt opt.txt opt_hash.txt output.txt runtime.png test test1 test2
