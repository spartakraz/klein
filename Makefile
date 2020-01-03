## Makefile for the Klein project

compiler=gcc
debug_flag=0
asm_options=-masm=intel
W_flags=-Wall -Wfatal-errors -Wno-array-bounds -Wno-discarded-qualifiers 

ifeq ($(debug_flag),1)
   custom_defines=-DDEBUG_MODE_ENABLED
   gcc_options=-g $(W_flags)
   output=klein-debug
else
   custom_defines=-DDEBUG_MODE_DISABLED
   gcc_options=-Os -funroll-loops $(W_flags)
   output=klein 
endif

bin_dir=bin
all: \
	klein_conio_c \
	klein_math_c \
	klein_utils_c \
	klein_errno_c \
	klein_line_c \
	klein_buffer_c \
	klein_config_c \
	klein_editor_c \
	klein_ui_c \
	klein_clipboard_c \
	klein_main_c
	    gcc	    \
		    $(bin_dir)/conio.o \
		    $(bin_dir)/math.o \
		    $(bin_dir)/utils.o \
		    $(bin_dir)/errno.o \
		    $(bin_dir)/line.o \
		    $(bin_dir)/buffer.o \
		    $(bin_dir)/config.o \
		    $(bin_dir)/editor.o \
		    $(bin_dir)/ui.o \
		    $(bin_dir)/clipboard.o \
		    $(bin_dir)/klein.o \
		    -o $(bin_dir)/$(output) -lcurses
	
klein_utils_c: utils.c library.h
	$(compiler)  -c $(gcc_options) $(asm_options) $(custom_defines)  utils.c -o $(bin_dir)/utils.o	
klein_line_c: line.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  line.c -o $(bin_dir)/line.o
klein_buffer_c: buffer.c library.h
	$(compiler)  -c $(gcc_options) $(asm_options)  $(custom_defines)  buffer.c -o $(bin_dir)/buffer.o
klein_conio_c: conio.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  conio.c -o $(bin_dir)/conio.o	
klein_editor_c: editor.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  editor.c -o $(bin_dir)/editor.o
klein_ui_c: ui.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  ui.c -o $(bin_dir)/ui.o
klein_config_c: config.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  config.c -o $(bin_dir)/config.o
klein_errno_c: errno.c library.h
	$(compiler)  -c $(gcc_options) $(asm_options)  $(custom_defines)  errno.c -o $(bin_dir)/errno.o	
klein_math_c: math.c library.h
	$(compiler)  -c $(gcc_options)  $(asm_options) $(custom_defines)  math.c -o $(bin_dir)/math.o	
klein_clipboard_c: clipboard.c library.h
	$(compiler)  -c $(gcc_options) $(asm_options)  $(custom_defines)  clipboard.c -o $(bin_dir)/clipboard.o		
klein_main_c: klein.c library.h
	$(compiler)  -c $(gcc_options) $(asm_options)  $(custom_defines)  klein.c -o $(bin_dir)/klein.o
	
clean:
	rm -rvf $(bin_dir)/*.o $(bin_dir)/$(output)
install:
	cp -v $(bin_dir)/$(output) /usr/bin/$(output)
uninstall:
	rm -v /usr/bin/$(output)

