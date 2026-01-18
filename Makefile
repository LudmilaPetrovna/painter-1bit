CC=/usr/bin/i686-w64-mingw32-gcc
STRIP=/usr/bin/i686-w64-mingw32-strip
CFLAGS=-ggdb3 -std=c99 -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter
LDFLAGS=-lgdi32
TARGET=simple_win32.exe

all:
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $(TARGET)


upload:
	$(STRIP) --strip-all $(TARGET)
	rm -f upload.7z
	7z a upload.7z $(TARGET) > /dev/null
	curl -F"file=@upload.7z" https://0x0.st
	rm -f upload.7z
