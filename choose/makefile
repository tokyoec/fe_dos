#
# makefile for Borland Borland C++ Compiler 5.5 Make
#
CC = bcc32 
CFLAGS = -5 -c -j1 -g5 -w32
LINK = bcc32
LFLAGS = -5 -w32 -e$(PROJ) -N
TARGET = choose
OBJS = choose.obj pc_model.obj lib_98.obj lib_ibm.obj lib_fmr.obj modify.obj utility.obj

.SUFFIXES: .obj .c .h

# $(TARGET).exe: $(OBJ)
# $(LINK) -e $@ $^

$(TARGET).lib: $(OBJS)
	tlib $(TARGET) +choose.obj +pc_model.obj +lib_98.obj +lib_ibm.obj +lib_fmr.obj +modify.obj +utility.obj

.c.obj:
	$(CC) -c $<

clean:
$(RM) $(TARGET) $(OBJS) *.tds *.bak
