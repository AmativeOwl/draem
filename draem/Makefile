GCC = gcc
FLAGS = -Wall -Wextra 
SRC = music.c test.c audio.c

music_test.exe: $(SRC) music.h audio.h
	$(GCC) $(FLAGS) $(SRC) -o music_test.exe -lwinmm

clean: 
	rm -f music_test.exe