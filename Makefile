SRCDIR = src

EXECUTABLES=pthread words lwords pwords_v1 pwords_v2

CC=gcc
CFLAGS=-g3 -pthread -Wall -std=gnu99 -I$(SRCDIR)
LDFLAGS=-pthread

.PHONY: all clean

all: $(EXECUTABLES)

UNAME := $(shell uname -m)
OBJ_SUFFIX := .o
ifeq "$(UNAME)" "arm64"
    OBJ_SUFFIX := _arm.o
endif
ifeq "$(UNAME)" "aarch64"
    OBJ_SUFFIX := _arm.o
endif

pthread: $(SRCDIR)/pthread.o
	$(CC) $(LDFLAGS) $^ -o $@

words: $(SRCDIR)/words$(OBJ_SUFFIX) $(SRCDIR)/word_helpers$(OBJ_SUFFIX) $(SRCDIR)/word_count$(OBJ_SUFFIX)
	$(CC) $(LDFLAGS) $^ -o $@

lwords: $(SRCDIR)/lwords$(OBJ_SUFFIX) $(SRCDIR)/word_count_l.o $(SRCDIR)/word_helpers$(OBJ_SUFFIX) $(SRCDIR)/list.o $(SRCDIR)/debug.o
	$(CC) $(LDFLAGS) $^ -o $@

pwords_v1: $(SRCDIR)/pwords_v1.o $(SRCDIR)/word_count_p.o $(SRCDIR)/word_helpers$(OBJ_SUFFIX) $(SRCDIR)/list.o $(SRCDIR)/debug.o
	$(CC) $(LDFLAGS) $^ -o $@

pwords_v2: $(SRCDIR)/pwords_v2.o $(SRCDIR)/word_count_p.o $(SRCDIR)/word_helpers$(OBJ_SUFFIX) $(SRCDIR)/list.o $(SRCDIR)/debug.o
	$(CC) $(LDFLAGS) $^ -o $@

$(SRCDIR)/word_count_l.o: $(SRCDIR)/word_count_l.c
	$(CC) $(CFLAGS) -DPINTOS_LIST -c $< -o $@

$(SRCDIR)/word_count_p.o: $(SRCDIR)/word_count_p.c
	$(CC) $(CFLAGS) -DPINTOS_LIST -DPTHREADS -c $< -o $@

$(SRCDIR)/pwords_v1.o: $(SRCDIR)/pwords.c
	$(CC) $(CFLAGS) -DPINTOS_LIST -DPTHREADS -DPWORDS_V1 -c $< -o $@

$(SRCDIR)/pwords_v2.o: $(SRCDIR)/pwords.c
	$(CC) $(CFLAGS) -DPINTOS_LIST -DPTHREADS -c $< -o $@

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.ONESHELL:
clean:
	tmp_dir=`mktemp -d`
	# Backup pre-compiled objects (lấy từ src/)
	cp $(SRCDIR)/words.o $(SRCDIR)/lwords.o $(SRCDIR)/word_count.o $(SRCDIR)/word_helpers.o $(SRCDIR)/lwords_arm.o $(SRCDIR)/word_count_arm.o $(SRCDIR)/word_helpers_arm.o $(SRCDIR)/words_arm.o $$tmp_dir 2>/dev/null || true
	
	# Xóa file thực thi ở root
	rm -f $(EXECUTABLES) 
	
	# Xóa file .o và .txt rác
	rm -f $(SRCDIR)/*.o *.o *.txt 
	
	# Restore lại pre-compiled objects vào src/
	cp $${tmp_dir}/*.o $(SRCDIR)/ 2>/dev/null || true
	rm -r $$tmp_dir