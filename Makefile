ARCH=spectrum
CC=zcc
CFLAGS_SPECCY=+zx -O3 -SO3 -clib=ansi
CFLAGS_AMSTRAD=+cpc -O3 -SO3 -clib=ansi

ifeq ($(ARCH), amstrad)
	CFLAGS=$(CFLAGS_AMSTRAD)
else
	CFLAGS=$(CFLAGS_SPECCY)
endif

all: bares bares-dsk

bares: bares.c player locs objs cmds
	$(CC) $(CFLAGS) -create-app bares.c player.obj locs.obj objs.obj cmds.obj -obares -lndos
	
bares-dsk: bares
	ifeq ($(ARCH), amstrad)
		$(CC) $(CFLAGS) -create-app -subtype=dsk bares
	endif

player: player.h player.c
	$(CC) $(CFLAGS) player.c -oplayer.obj
	
locs: locs.h locs.c
	$(CC) $(CFLAGS) locs.c -olocs.obj
	
objs: objs.h objs.c
	$(CC) $(CFLAGS) objs.c -oobjs.obj
	
cmds: cmds.h cmds.c
	$(CC) $(CFLAGS) cmds.c -ocmds.obj
	
clean:
	$(RM) *.obj
	$(RM) bares
	$(RM) *.tap
	$(RM) *.cpc
	$(RM) *.reloc

run:
	fuse bares.tap
