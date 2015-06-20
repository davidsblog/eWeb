CC=gcc
CFLAGS=-pthread -std=gnu99
CFILES=eweb.c dwebsvr.c
RESOURCES=index.h smoothie.h code.h jquery-2-1-0-min.h
TARGET=eWeb

$(TARGET): $(CFILES) $(RESOURCES)
	$(CC) $(CFILES) $(CFLAGS) -o $@

index.h: index.html
	xxd -i index.html index.h

smoothie.h: smoothie.js
	xxd -i smoothie.js smoothie.h

code.h: code.js
	xxd -i code.js code.h

jquery-2-1-0-min.h: jquery-2-1-0-min.js
	xxd -i jquery-2-1-0-min.js jquery-2-1-0-min.h

.PHONY: clean

clean:
	rm -f *.o $(TARGET) $(RESOURCES)
