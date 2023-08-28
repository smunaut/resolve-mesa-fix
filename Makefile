CC = gcc
CFLAGS = -Wall -Werror -fPIC
LDLIBS = -ldl
LDFLAGS = -shared

resolve_mesa_fix.so: resolve_mesa_fix.o
	$(CC) -o $@ $(LDLIBS) $(LDFLAGS) $<

clean:
	rm -f *.o *.so
