all: normalmat
    
normalmat: normalmat.c
	gcc $< -lm -o $@



clean:
	rm -f normalmat

run: clean all
	./normalmat
