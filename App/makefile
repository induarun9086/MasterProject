all: imgProc/utils/misc.c imgProc/utils/matrix.c imgProc/test/test.c imgProc/fileOps/bmp.c imgProc/imgOps/RGBToGray.c imgProc/imgOps/imgUtils.c imgProc/imgOps/imfilter.c imgProc/templatematching.c
	rm -f ./imgProc/temp/*.bmp
	rm -rf ./imgProc/bin
	rm -f ./imgProc/*.bmp
	rm -f ./*.bmp
	mkdir ./imgProc/bin
	gcc -std=c99 -g -lm -o ./imgProc/bin/imgProc imgProc/utils/misc.c imgProc/utils/matrix.c imgProc/test/test.c imgProc/fileOps/bmp.c imgProc/imgOps/RGBToGray.c imgProc/imgOps/imgUtils.c imgProc/imgOps/imfilter.c imgProc/templatematching.c

