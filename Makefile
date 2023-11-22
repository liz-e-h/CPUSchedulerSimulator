SOURCES = main.cpp scheduler.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
LDLIBS = 
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = scheduler

all: $(TARGET)

deadlock_detector.o:  scheduler.h
main.o:  scheduler.h
%.o : %.c
$(OBJECTS): Makefile 

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(OBJECTS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f .*~ *~ *.o $(TARGET)
