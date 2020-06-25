TARGET = memctl.elf
CROSS_COMPILER =
SLIB = argparse/libargparse.a
INC = -I. -I./argparse
CFLAGS = -g -Wall

$(TARGET): main.c Makefile $(SLIB)
	$(CROSS_COMPILER)gcc $(CFLAGS) $(INC) main.c $(SLIB) -o $(TARGET)

$(SLIB):
	make -C argparse

clean:
	rm -f $(TARGET)
	make -C argparse clean

.PHONY: clean
