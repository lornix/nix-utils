#
# make all projects in subdirs
#
SUBDIRS=$(wildcard */)
#
.PHONY: all $(SUBDIRS) clean
#
all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $${dir} clean ; done
