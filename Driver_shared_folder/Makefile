# Module name
obj-m := my_module.o

# Source files
my_module-objs := main.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default rule to build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean rule to remove generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
