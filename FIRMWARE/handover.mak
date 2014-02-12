export PATH:=$(ffroot)/_dev_tools;$(PATH)
#NOTE: here any flavor of GNU make will find a unix-like shell and will use it as default. Be aware!

clean:
    make clean_ff_svi_$(TargetSuffix) MAKEFLAGS=$(MAKEFLAGS)

build:
    echo $(EMBOS)
    echo $(ffroot)
    make ff_svi_$(TargetSuffix) MAKEFLAGS=$(MAKEFLAGS)
