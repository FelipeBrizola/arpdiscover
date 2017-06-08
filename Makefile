all:
	gcc -o arpdiscover arpdiscover.c -lm
	./arpdiscover

clean:
	rm -f arpdiscover
