CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude 

OBJDIR = exe

# Linking step 
exe/test_basic : $(OBJDIR)/test_basic.o $(OBJDIR)/allocator.o
	$(CC) $^ -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : tests/%.c include/allocator.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/allocator.o : src/allocator.c include/allocator.h | $(OBJDIR) # | $(OBJDIR) makes it a order only dependancie i.e. changes made in it should not recompile the target, also this must exists befoure compiling
	$(CC) $(CFLAGS) -c $< -o $@
