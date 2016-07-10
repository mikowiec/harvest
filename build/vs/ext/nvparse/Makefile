MAIN=libnvparse.so

CC=g++

CFLAGS= -DUNIX -Wall -g

INCLUDES= -I../../../include/glh -I../../../include/shared -I/usr/X11R6/include 

LFLAGS=  -L/usr/X11R6/lib -lGL -lGLU -lglut  

SRCS =  nvparse.cpp nvparse_errors.cpp rc1.0_combiners.cpp rc1.0_final.cpp rc1.0_general.cpp ts1.0_inst.cpp ts1.0_inst_list.cpp _ts1.0_parser.cpp _ts1.0_lexer.cpp _rc1.0_lexer.cpp _rc1.0_parser.cpp _vs1.0_lexer.cpp _vs1.0_parser.cpp _ps1.0_lexer.cpp _ps1.0_parser.cpp vs1.0_inst.cpp  vs1.0_inst_list.cpp ps1.0_program.cpp 

OBJS = $(SRCS:.cpp=.o)
$(MAIN):	
	-bison -d -o _ps1.0_parser -p ps10_ ps1.0_grammar.y
	-mv -f _ps1.0_parser _ps1.0_parser.cpp
	-bison -d -o _rc1.0_parser -p rc10_ rc1.0_grammar.y
	-mv -f _rc1.0_parser _rc1.0_parser.cpp
	-bison -d -o _ts1.0_parser -p ts10_ ts1.0_grammar.y
	-mv -f _ts1.0_parser _ts1.0_parser.cpp
	-bison -d -o _vs1.0_parser -p vs10_ vs1.0_grammar.y
	-mv -f _vs1.0_parser _vs1.0_parser.cpp
	-flex -Prc10_ -o_rc1.0_lexer.cpp rc1.0_tokens.l
	-flex -Pps10_ -o_ps1.0_lexer.cpp ps1.0_tokens.l
	-flex -Pts10_ -o_ts1.0_lexer.cpp ts1.0_tokens.l
	-flex -Pvs10_ -o_vs1.0_lexer.cpp vs1.0_tokens.l
	make compile
compile: $(OBJS)
	$(CC) -shared -Wl,-soname,libnvparse.so $(CFLAGS) $(OBJS) -o $(MAIN) $(LFLAGS)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES)   -c $<

clean:
	rm -f *.o *~ $(MAIN)

install:
	make
	install -c -m644 $(MAIN) $(ROOT)/usr/local/lib
