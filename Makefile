OBJS = led.o
CC = aarch64-linux-gnu-gcc
MFLAGS = -ldl -lpthread
RFLAGS = -lwiringPi -lcrypt

all: webserver librasp.so index.html

webserver: webserver.o
	$(CC) -o $@ webserver.o $(MFLAGS)
	scp webserver taejeong@100.82.123.25:~/

librasp.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(RFLAGS)
	scp librasp.so taejeong@100.82.123.25:~/

index.html:
	scp index.html taejeong@100.82.123.25:~/

%.o: %.c
	$(CC) -c -fPIC $<

clean:
	rm -f *.o webserver librasp.so