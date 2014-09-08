CPP_FILES= $(wildcard *.cpp)
LIBS=`sdl2-config --libs` -lGL
CXXFLAGS=-std=c++11 -I./ -Iinclude -Isrc

TARGET=loader
RUNSTRING=./${TARGET}       #it is possible to invoke "make run"

OBJECTS = $(CPP_FILES:.cpp=.o)    #Replaces cpp with object

#release target (standard)
release: CXXFLAGS += -Ofast
release: all


all: .depend ${TARGET}

#debug configuration
debug: CXXFLAGS += -g -O0
debug: all

help:
	@cat INSTALL
	@echo

#Calculating dependencies
.depend: ${CPP_FILES} Makefile
	@echo
	@echo Calculating dependencies
	$(CXX) $(CFLAGS) -MM ${CPP_FILES} ${CXXFLAGS} > ./.dependtmp
	cat ./.dependtmp | sed 's/h$$/h \n\t \$(CXX) $$< -o $$@ -c $$(CXXFLAGS)/' > ./.depend
	rm -f ./.dependtmp

${TARGET}: ${OBJECTS} ${CPP_FILES} ${JACK_OBJECT}
	@echo
	@echo Linking ...
	${CXX} ${FLAGS} -o ${TARGET} ${OBJECTS} ${JACK_FLAGS} ${LIBS}

include .depend

#För att kunna köra filen direkt
run: ${TARGET}
	${RUNSTRING}

clean:
	rm -f ${OBJECTS} ${ENGINE_OBJECTS}
	rm -f .depend

rebuild: clean ${TARGET}

deb:
	debuild -b

