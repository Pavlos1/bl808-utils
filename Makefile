all: print_boot_header gen_boot_header

print_boot_header:	print_boot_header.o util.o
gen_boot_header:	gen_boot_header.o util.o fw_header.o

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

clean:
	rm -f print_boot_header gen_boot_header *.o *.d

-include	$(OBJS:.o=.d)
