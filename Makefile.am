bin_PROGRAMS = hannah
# Set the variable needed by pacman.cc for iresource location
DATA_INSTALL_DIR=$(datadir)/@PACKAGE@
# Add the -D parameter to /usr/bin/install
INSTALL=@INSTALL@ -D
# Files to be included in a "make dist" should be included here, as well as anything needed to make the program compile
hannah_SOURCES = Defines.h Global.cc Global.h ScoreLib.cc ScoreLib.h Level.cc Level.h Animation.cc Animation.h \
		AnimationFactory.cc AnimationFactory.h Sprite.cc Sprite.h Bonus.cc Bonus.h Food.cc Food.h \
		Player.cc Player.h Ghost.cc Ghost.h Ghost1.cc Ghost1.h Ghost2.cc Ghost2.h Ghost3.cc Ghost3.h Menu.cc Menu.h pacman.cc
nodist_hannah_SOURCES = filelist.sh
# Additional CFLAGS
AM_CPPFLAGS = $(SDL_CFLAGS) -DDATA_INSTALL_DIR=\"$(DATA_INSTALL_DIR)\"
# LIBS comes after the G++ sources, so library files need to go here
LIBS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf)
# the create_file_list.sh script generates the below include file listing all files and their output directories
include hannah_files.mk
# Additional files that need to be included in a release
EXTRA_DIST=resources
