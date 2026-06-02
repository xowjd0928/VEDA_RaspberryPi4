OBJS = led.o cds.o buzzer.o segment.o
ARM_CC = aarch64-linux-gnu-gcc
MFLAGS = -ldl -lpthread
RFLAGS = -lwiringPi -lcrypt

all: webserver librasp.so index client

webserver: webserver.o
	$(ARM_CC) -o $@ webserver.o $(MFLAGS) $(RFLAGS)
	-scp webserver taejeong@100.82.123.25:~/

librasp.so: $(OBJS)
	$(ARM_CC) -shared -o $@ $(OBJS) $(RFLAGS)
	-scp librasp.so taejeong@100.82.123.25:~/

index:
	-scp index.html taejeong@100.82.123.25:~/
	-scp style.css taejeong@100.82.123.25:~/

client: client.c
	$(CC) -o $@ client.c

%.o: %.c
	$(ARM_CC) -c -fPIC $<

clean:
	rm -f *.o webserver librasp.so client