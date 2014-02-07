CPPFLAGS :=
CFLAGS := -Wall -ggdb
CXXFLAGS := -fno-exceptions -fno-rtti
ASMFLAGS := -fverbose-asm -dP
flags := -O3
SHELL := bash
ARGS :=
V := @

all: x.o.report a.out.report
	-$(V)for f in $+; do echo -n "$$f: "; cat "$$f"; done

x.o: x.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(flags) $<
x.s: x.cpp
	$(CXX) -S -o $@ $(CPPFLAGS) $(ASMFLAGS) $(CFLAGS) $(CXXFLAGS) $(flags) $<
x.E: x.cpp
	$(CXX) -E -o $@ $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(flags) $<
-include x.d
%.d: %.cpp
	$(CPP) -MM -MG -o $@ -MQ '$(patsubst %.cpp,%.o,$<)' $< $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(flags)
a.out: x.o
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(flags) -o $@ $+

%.report: %
	-$(V)test -f $<.prevtext && prevtext=$$(< $<.prevtext);\
	    test -n "$$prevtext" || prevtext=0;\
	    test -f $<.prevdec && prevdec=$$(< $<.prevdec);\
	    test -n "$$prevdec" || prevdec=0;\
	    test -f $<.prevdata && prevdata=$$(< $<.prevdata);\
	    test -n "$$prevdata" || prevdata=0;\
	    test -f $<.prevbss && prevbss=$$(< $<.prevbss);\
	    test -n "$$prevbss" || prevbss=0;\
	    size $< | \
	    grep -v text | \
	    while read text data bss dec rest; do \
	       printf 'text:%d%+d data:%d%+d bss:%d%+d dec:%d%+d\n' \
	       $$text $$(($$text - $$prevtext)) \
	       $$data $$(($$data-$$prevdata))\
	       $$bss $$(($$bss-$$prevbss))\
	       $$dec $$(($$dec-$$prevdec)); \
	       echo "$$text" >$<.prevtext; \
	       echo "$$data" >$<.prevdata; \
	       echo "$$bss" >$<.prevbss; \
	       echo "$$dec" >$<.prevdec; \
	    done > $@

valgrind: a.out
	valgrind --tool=memcheck --leak-check=full $(abspath $<) $(ARGS)
gdb:
	-tmp=$$(tempfile -p gdb || echo gdb$$$$.tmp) && echo run >"$$tmp"; \
	test -f "$$tmp" && opt="-x $$tmp";\
	gdb -q $$opt --args a.out $(ARGS);\
	rc=$$?;\
	rm -f "$$tmp";\
	exit $$rc
.PHONY: valgrind gdb all
