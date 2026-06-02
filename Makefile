OBJS = led.o
CC = aarch64-linux-gnu-gcc
MFLAGS = -ldl -lpthread
RFLAGS = -lwiringPi -lcrypt

all: webserver librasp.so upload

webserver: webserver.o
	$(CC) -o $@ webserver.o $(MFLAGS)

librasp.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(RFLAGS)

.PHONY: upload

upload:
	scp webserver taejeong@100.82.123.25:~/
	scp librasp.so taejeong@100.82.123.25:~/
	scp index.html taejeong@100.82.123.25:~/

%.o: %.c
	$(CC) -c -fPIC $<

clean:
	rm -f *.o webserver librasp.so