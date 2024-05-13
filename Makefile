LDFLAGS = -Wl,-rpath=.,-export-dynamic
INCLUDES = -I./raylib-5.0/src -I./cLDtk
LIBRAYLIB = ./raylib-5.0/src/libraylib.a
LIBCLDTK = ./cLDtk/libcLDtk.a
LIBS = -lm

MODULE_SRCS = module.c \
			  track_editor.c \
			  mymath.c \
			  debug.c \
			  kart.c \
			  spritesheet.c \
			  tiles.c \
			  $(LIBCLDTK) \
			  dialog.c \
			  text.c \
			  shared.c \
			  collision.c \
			  animation/animator.c \
			  animation/animations.c \
			  overworld/npc.c \
			  overworld/overworld.c \
			  overworld/overworld_player.c

main: main.c module raylib cLDtk
	$(CC) $(LIBS) $(LDFLAGS) $(INCLUDES) -g -o $@ $< track_editor.c mymath.c debug.c $(LIBRAYLIB)

.PHONY: module
module: libmodule.so
libmodule.so: $(MODULE_SRCS)
	$(CC) $(LIBS) $(LDFLAGS) $(INCLUDES) -g -fPIC -shared -o $@ $^

.PHONY: raylib
raylib: $(LIBRAYLIB)
$(LIBRAYLIB):
	make PLATFORM=PLATFORM_DESKTOP -C ./raylib-5.0/src

./cLDtk/cLDtk.o: ./cLDtk/cLDtk.c
	$(CC) -g -c -fPIC -o $@ $^ -std=c99

./cLDtk/parson.o: ./cLDtk/parson.c
	$(CC) -g -c -fPIC -o $@ $^ -std=c99

.PHONY: cLDtk
cLDtk: $(LIBCLDTK)
$(LIBCLDTK): ./cLDtk/cLDtk.o ./cLDtk/parson.o
	ar rvs $(LIBCLDTK) ./cLDtk/cLDtk.o ./cLDtk/parson.o

test_math: test/test_math.c
	$(CC) $(INCLUDES) -g -o $@ $^ -lm

test_text: test/test_text.c $(LIBRAYLIB) text.c
	$(CC) $(INCLUDES) -g -o $@ $^ -lm

.PHONY: clean
clean:
	rm -f main libmodule.so
	make clean -C raylib-5.0/src
	rm -f ./cLDtk/*.o ./cLDtk/libcLDtk.a
