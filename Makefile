ALLEGRO_VERSION=5.0.10
PATH_ALLEGRO=C:\allegro-$(ALLEGRO_VERSION)-mingw-4.7.0
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: plates.exe 

plates.exe: plates.o 
	gcc -o plates.exe plates.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)	
	
plates.o: plates.c 
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c plates.c

clean:
	del plates.o 
	del plates.exe