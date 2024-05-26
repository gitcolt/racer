LDFLAGS = -Wl,-rpath=.,-export-dynamic
INCLUDES = -I./raylib-5.0/src -I./cLDtk
LIBRAYLIB = ./raylib-5.0/src/libraylib.a
LIBCLDTK = ./cLDtk/libcLDtk.a
LIBS = -lm

MODULE_SRCS = module.c \
			  debug.c \
			  kart.c \
			  spritesheet.c \
			  tiles.c \
			  collision.c \
			  xml.c \
			  sxml.c \
			  shared/mymath.c \
			  shared/shared.c \
			  animation/animator.c \
			  animation/animations.c \
			  dialog/dialog.c \
			  dialog/dialog_parser.c \
			  dialog/text.c \
			  overworld/npc.c \
			  overworld/overworld.c \
			  overworld/overworld_player.c \
			  track_editor/track_editor.c \
			  race/race.c \
			  $(LIBCLDTK)

main: main.c module raylib
	@$(CC) $(LIBS) $(LDFLAGS) $(INCLUDES) -g -o $@ $< shared/mymath.c debug.c $(LIBRAYLIB)

.PHONY: module
module: libmodule.so
libmodule.so: $(MODULE_SRCS) $(LIBCLDTK)
	@$(CC) $(LIBS) $(LDFLAGS) $(INCLUDES) -g -fPIC -shared -o $@ $^

.PHONY: raylib
raylib: $(LIBRAYLIB)
$(LIBRAYLIB):
	make PLATFORM=PLATFORM_DESKTOP -C ./raylib-5.0/src

./cLDtk/cLDtk.o: ./cLDtk/cLDtk.c
	@$(CC) -g -c -fPIC -o $@ $^ -std=c99

./cLDtk/parson.o: ./cLDtk/parson.c
	@$(CC) -g -c -fPIC -o $@ $^ -std=c99

.PHONY: cLDtk
cLDtk: $(LIBCLDTK)
$(LIBCLDTK): ./cLDtk/cLDtk.o ./cLDtk/parson.o
	@ar rvs $(LIBCLDTK) ./cLDtk/cLDtk.o ./cLDtk/parson.o

test_math: test/test_math.c
	$(CC) $(INCLUDES) -g -o $@ $^ -lm

test_text: test/test_text.c $(LIBRAYLIB) text.c
	$(CC) $(INCLUDES) -g -o $@ $^ -lm

.PHONY: clean
clean:
	rm -f main libmodule.so
	make clean -C raylib-5.0/src
	rm -f ./cLDtk/*.o ./cLDtk/libcLDtk.a
