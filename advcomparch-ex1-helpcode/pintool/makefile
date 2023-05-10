# If the tool is built out of the kit, PIN_ROOT must be specified in the make invocation and point to the kit root.
PIN_ROOT ?= /home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/pin-3.27-98718-gbeaa5d51e-gcc-linux
ifdef PIN_ROOT
CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
else
CONFIG_ROOT := ../Config
endif
include $(CONFIG_ROOT)/makefile.config
include makefile.rules
include $(TOOLS_ROOT)/Config/makefile.default.rules
