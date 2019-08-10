.PHONY := all clean

include config.cfg

ifeq ($(DEBUG), n)
.SILENT:
endif

all:
	$(MAKE) -C src/ $@

clean:
	$(RM) disk.img bin/* disk/*.bin disk/system/kernel/*.bin
	$(MAKE) -C src/ $@