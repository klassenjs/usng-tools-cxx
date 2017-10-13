
BUILD_PG=YES
BUILD_PYTHON=YES
BUILD_RUBY=YES
BUILD_TESTS=YES

SUBDIRS = lib pg tests

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@


SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))
.PHONY: clean $(SUBCLEAN)

clean: $(SUBCLEAN)

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean
