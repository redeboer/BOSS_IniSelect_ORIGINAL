# * Author: Remco de Boer <remco.de.boer@ihep.ac.cn>
# * Date: January 11th, 2019
# * Based on the NIKHEFproject2018 repository
# * For explanations, see https://www.gnu.org/software/make/manual/make.html


# * PATH DEFINITIONS * #
SUFFIXES += .d
DEPDIR := .d
INCDIR := inc
SRCDIR := src
SCRDIR := scripts
BINDIR := bin
EXEDIR := exe
INCLUDE_PATH = -I${INCDIR}
LIBNAME = BossAfterburner
OUTLIBF = lib${LIBNAME}.a


# * PREPARE DIRECTORIES * #
$(shell mkdir -p $(DEPDIR) >/dev/null)


# * COMPILER FLAGS * #
CC = g++
# CC = clang++
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
OBJ_BIN  = $(OBJECTS:.cxx=.o)
OBJ_BIN := $(addprefix ${BINDIR}/, ${OBJ_BIN})

# * for the scripts (executables)
SCRIPTS  = $(notdir $(wildcard ${SCRDIR}/*.C))
SCR_BIN  = $(SCRIPTS:.C=.exe)
SCR_BIN := $(addprefix ${EXEDIR}/, ${SCR_BIN})

# * for the dependencies
DEPENDS  = $(notdir $(wildcard ${DEPDIR}/*.d))

# * Compile objects: the parametesr are the rules defines below and above
.PHONY: all
all : ${OBJ_BIN} $(OUTLIBF) ${SCR_BIN}
	@echo "COMPILING DONE"


# * COMPILE RULES * #
# * (dependencies are constructed too)
# * for the objects (inc and src)
${BINDIR}/%.o : ${SRCDIR}/%.cxx ${INCDIR}/%.h $(DEPDIR)/%.d
	@echo "Compiling object \"$(notdir $<)\""
	@mkdir -p $(@D) > /dev/null
	@$(CC) $(CFLAGS) ${INCLUDE_PATH} ${DEPFLAGS} -c $< -o $@
	$(POSTCOMPILE)

# * for linking the objects generated above.
$(OUTLIBF) : ${OBJ_BIN}
	@ar rU $@ $^ > /dev/null
	@ranlib $@

# * for the scripts (executables)
${EXEDIR}/%.exe : ${SCRDIR}/%.C $(DEPDIR)/%.d $(OUTLIBF)
	@echo "Compiling script \"$(notdir $<)\""
	@mkdir -p $(@D) > /dev/null
	@$(CC) $< -o $@ ${CFLAGS} ${INCLUDE_PATH} ${DEPFLAGS} -L. -l${LIBNAME} ${LFLAGS}
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