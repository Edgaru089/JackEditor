
.SILENT:
include Makefile.flags

output := jack_editor

objects      = $(patsubst %.c,%.o,$(shell find . -name "*.c")) $(patsubst %.cpp,%.o,$(shell find . -name "*.cpp"))
objects_fasm = $(patsubst %.S,%.o,$(shell find . -name "*.S"))

objects_test = $(patsubst %.c,%.o,$(shell find . -name "test_*.c")) $(patsubst %.cpp,%.o,$(shell find . -name "test_*.cpp"))
objects     := $(filter-out $(objects_test),$(objects))


all: $(output)


$(output): $(objects) $(objects_fasm)
	$(LD) -o $@ $(LDFLAGS) $(objects) $(objects_fasm) $(LDLIBS)


clean:
	echo " -RM    $(output) $(objects)"
	-$(RM) -f $(output) $(objects) $(objects_fasm)


.PHONY: all clean install upload


