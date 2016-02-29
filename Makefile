CC ?= gcc
CFLAGS_common ?= -Wall -std=gnu99
CFLAGS_orig = -O0
CFLAGS_opt  = -O0 -g

EXEC = phonebook_orig phonebook_opt phonebook_opt_hash
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

phonebook_opt_hash: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

run: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_orig && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run1: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt && echo 3 | sudo tee /proc/sys/vm/drop_caches"
run2: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash && echo 3 | sudo tee /proc/sys/vm/drop_caches"

cache-test: $(EXEC)
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		./phonebook_orig 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		./phonebook_opt 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		./phonebook_opt_hash 1>/dev/null

report: $(EXEC)
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -F 12500 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		-o perf.orig ./phonebook_orig
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -F 12500 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		-o perf.opt ./phonebook_opt
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf record -F 12500 \
		-e cache-misses,cache-references,instructions,cycles,\
L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,\
L1-icache-load-misses,branch-misses,branch-instructions,page-faults \
		-o perf.opt_hash ./phonebook_opt_hash

output.txt: cache-test calculate
	./calculate

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o perf.* \
		calculate orig.txt opt.txt opt_hash.txt output.txt runtime.png
