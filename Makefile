all:
	gcc -o arpdiscover arpdiscover.c -lm
	chmod 777 *

clean:
	rm -f arpdiscover
