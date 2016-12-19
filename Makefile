install:
	test ! -d /usr/share/ipstatd && mkdir /usr/share/ipstatd
	cc  -o /usr/share/ipstatd/ipstatd ipstatd.c
uninstall:
	rm -r /usr/share/ipstatd
clean:
	rm -r /usr/share/ipstatd
