DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

MAKEFILES := $(shell find . -mindepth 2 -name Makefile)

#---------------------------------------------------------------------------------
all:
#---------------------------------------------------------------------------------
	@for i in $(MAKEFILES); do $(MAKE) -C `dirname $$i` || exit 1; done;
	@rm -fr bin
	@mkdir -p bin
	@find . -name "*.dol" ! -path "./bin/*" -exec cp -fv {} bin \;

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	@rm -fr bin
	@rm -f *.bz2
	@for i in $(MAKEFILES); do $(MAKE) -C `dirname $$i` clean || exit 1; done;

#---------------------------------------------------------------------------------
dist: clean
#---------------------------------------------------------------------------------
	@tar -cvjf gc-demos-$(DATESTRING).tar.bz2 *
