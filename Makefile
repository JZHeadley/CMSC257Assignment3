all: normalmat
    
normalmat: normalmat.c
	gcc $< -lm -g -o $@



clean:
	rm -f normalmat

run: clean all
	./normalmat 1000 10
