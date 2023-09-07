CC = gcc

CFLAGS = -Wall -O3
LDFLAGS = -lgmp

UTIL_SOURCES = cfinite.c utils.c
PTF_SOURCES = ptf.c $(UTIL_SOURCES)
PTC_SOURCES = ptc.c $(UTIL_SOURCES)

all: ptf ptc

128: CFLAGS += -DUSE_128
128: ptf ptc

ptf: $(PTF_SOURCES)
	$(CC) $(CFLAGS) -o ptf $(PTF_SOURCES) $(LDFLAGS)

ptc: $(PTC_SOURCES)
	$(CC) $(CFLAGS) -o ptc $(PTC_SOURCES) $(LDFLAGS)

clean:
	rm -f ptf ptc

