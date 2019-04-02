# * Author: Remco de Boer <remco.de.boer@ihep.ac.cn>
# * Date: January 11th, 2019
# * Based on the NIKHEFproject2018 repository
# * For explanations, see https://www.gnu.org/software/make/manual/make.html
# TODO: implent auto dependency generation, see http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/


# * PATH DEFINITIONS * #
DEPDIR := .d
INCDIR := inc
SRCDIR := src
BINDIR = bin
EXEDIR = exe
INCLUDE_PATH = -I${INCDIR}
LIBNAME = BossAfterburner
LIBRARY = lib${LIBNAME}.a


# * PREPARE DIRECTORIES * #
$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(BINDIR) >/dev/null)
$(shell mkdir -p $(EXEDIR) >/dev/null)


# * COMPILER FLAGS * #
SUFFIXES += .d
COMPILER = g++
# COMPILER = clang++
ROOTINC    := -I$(shell root-config --incdir)
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs --evelibs --glibs)
CFLAGS = -fPIC -w -g -W ${ROOTCFLAGS} -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
LFLAGS = ${ROOTLIBS} -g -lGenVector  -lRooFit -lRooFitCore -lRooStats -lMinuit
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@


# * INVENTORIES OF OBJECTS AND THEIR EVENTUAL BINARIES * #
# * for the objects (inc and src)
OBJECTS  = $(notdir $(wildcard ${SRCDIR}/*.cxx))
# $(shell find src/ -name "*.cxx")
OBJ_BIN  = $(OBJECTS:.cxx=.o)
OBJ_BIN := $(addprefix ${BINDIR}/, ${OBJ_BIN})

# * for the scripts (executables)
SCRIPTS  = $(notdir $(wildcard scripts/*.C))
SCR_BIN  = $(SCRIPTS:.C=.exe)
SCR_BIN := $(addprefix ${EXEDIR}/, ${SCR_BIN})

# * for the dependencies
DEPENDS  = $(notdir $(wildcard ${DEPDIR}/*.d))

# * Compile objects: the parametesr are the rules defines below and above
.PHONY: all
all : ${OBJ_BIN} $(LIBRARY) ${SCR_BIN}
	@echo "COMPILING DONE"


# * COMPILE RULES * #
# * (dependencies are constructed too)
# * for the objects (inc and src)
${BINDIR}/%.o : ${SRCDIR}/%.cxx ${INCDIR}/%.h $(DEPDIR)/%.d
	@echo "Compiling object \"$(notdir $<)\""
	@$(COMPILER) $(CFLAGS) ${INCLUDE_PATH} ${DEPFLAGS} -c $< -o $@
	$(POSTCOMPILE)
# @mv -f ${DEPDIR}/$*.d ${DEPDIR}/$*.d.tmp
# @sed -e 's|.*:|$*.o:|' < ${DEPDIR}/$*.d.tmp > ${DEPDIR}/$*.d
# @sed -e 's/.*://' -e 's/\\$$//' < ${DEPDIR}/$*.d.tmp | fmt -1 | \
# 	sed -e 's/^ *//' -e 's/$$/:/' >> ${DEPDIR}/$*.d
# @rm -f ${DEPDIR}/$*.d.tmp

# * for linking the objects generated above.
$(LIBRARY) : $(shell find bin/ -name "*.o")
	ar ru $@ $^
	ranlib $@

# * for the scripts (executables)
${EXEDIR}/%.exe : scripts/%.C $(DEPDIR)/%.d $(LIBRARY)
	@echo "Compiling script \"$(notdir $<)\""
	@$(COMPILER) $< -o $@ ${CFLAGS} ${INCLUDE_PATH} ${DEPFLAGS} -L. -l${LIBNAME} ${LFLAGS}
	$(POSTCOMPILE)

# * for the dependencies
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

# * REMOVE ALL BINARIES * #
# * This rule can be called using "make clean"
.PHONY : clean
clean:
	$(RM) lib${LIBNAME}.a ${BINDIR}/* ${EXEDIR}/* ${DEPDIR}/*

# Include all .d files
-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))