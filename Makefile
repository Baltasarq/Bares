CC=zcc
CFLAGS=+zx -O3 -SO3 -clib=ansi

all: bares

bares: bares.c player locs objs cmds
	$(CC) $(CFLAGS) -create-app bares.c player.o locs.o objs.o cmds.o -obares -lndos
	
player: player.h player.c
	$(CC) $(CFLAGS) -c player.c -oplayer.o
	
locs: locs.h locs.c
	$(CC) $(CFLAGS) -c locs.c -olocs.o
	
objs: objs.h objs.c
	$(CC) $(CFLAGS) -c objs.c -oobjs.o
	
cmds: cmds.h cmds.c
	$(CC) $(CFLAGS) -c cmds.c -ocmds.o
	
clean:
	$(RM) *.o
	$(RM) bares
	$(RM) *.tap
	$(RM) *.cpc
	$(RM) *.dsk
	$(RM) *.bin
	$(RM) *.reloc

run:
	fuse bares.tap
