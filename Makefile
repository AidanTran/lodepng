##### Source files and executable ############################################

SRCS 		= main.cpp Huffman.cpp lodepng.cpp raw.cpp

OBJECTS 	= main.o Huffman.o lodepng.o raw.o

EXECNAME 	= huffman

##### Libraries and paths ####################################################

LIBS            = 
INCDIRS 	= 
LIBDIRS 	= 
 
##### Compiler information ###################################################

CPP		= g++
CPPFLAGS 	= -Wall -Wextra -pedantic -ansi -O3 -std=c++11

##### Target compilation #####################################################

all:$(EXECNAME)

$(EXECNAME): 	$(OBJECTS)
	$(CPP) $(CPPFLAGS) $(LIBDIRS) $^ $(LIBS) -o $(EXECNAME) 

.cpp.o:	
	$(CPP) $(CPPFLAGS) $(INCDIRS) -c $<

.cc.o:	
	$(CPP) $(CPPFLAGS) $(INCDIRS) -c $<

clean:
	rm -rf *~ *.o *.a $(EXECNAME) 

##############################################################################