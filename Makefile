DESTDIR=/usr/local
MANDIR=/usr/share/man
CFLAGS=-std=c99
TRIG_PERCENT=5

all: hibbert doc shibbert
shibbert:
	cc -DTRIG_PERCENT=$(TRIG_PERCENT) shibbert.c -o shibbert
doc:
	gzip -k -f hibbert.1
	gzip -k -f shibbert.1
install: hibbert doc shibbert
	install hibbert $(DESTDIR)/bin
	install shibbert $(DESTDIR)/bin
	install hibbert.1.gz $(MANDIR)/man1/
	if [ ! -e /etc/hibbertt ]; then install hibbertt /etc/; fi
clean:
	rm -f hibbert hibbert.1.gz shibbert shibbert.1.gz
