OBJS = led.o cds.o buzzer.o segment.o
CC = aarch64-linux-gnu-gcc
MFLAGS = -ldl -lpthread
RFLAGS = -lwiringPi -lcrypt

all: webserver librasp.so index

webserver: webserver.o
	$(CC) -o $@ webserver.o $(MFLAGS)
	-scp webserver taejeong@100.82.123.25:~/

librasp.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(RFLAGS)
	-scp librasp.so taejeong@100.82.123.25:~/

index:
	-scp index.html taejeong@100.82.123.25:~/

%.o: %.c
	$(CC) -c -fPIC $<

clean:
	rm -f *.o webserver librasp.so