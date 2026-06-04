OBJS = led.o cds.o buzzer.o segment.o
CC = gcc
ARM_CC = aarch64-linux-gnu-gcc
MFLAGS = -ldl -lpthread
RFLAGS = -lwiringPi -lcrypt
USER = taejeong
HOST = 100.82.123.25
REMOTE = $(USER)@$(HOST):~/

all: webserver librasp.so client ui

webserver: webserver.o
	$(ARM_CC) -o $@ webserver.o $(MFLAGS) $(RFLAGS)
	-scp webserver $(REMOTE)
	
webserver.o: webserver.c
	$(ARM_CC) -c $<

librasp.so: $(OBJS)
	$(ARM_CC) -shared -o $@ $(OBJS) $(RFLAGS)
	-scp librasp.so $(REMOTE)

client: client.o
	$(CC) -o $@ client.c $(MFLAGS)

client.o: client.c
	$(CC) -c $<

ui:
	-scp index.html $(REMOTE)
	-scp style.css $(REMOTE)

%.o: %.c
	$(ARM_CC) -c -fPIC $<

clean:
	rm -f *.o webserver librasp.so client