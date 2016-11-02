INSTALL_DIR ?= $(CURDIR)/lib

LIBNAME			:= libHAL.so

current_dir	:= $(CURDIR)
IDIR				:= $(current_dir)/include
ODIR				:= $(current_dir)/lib
LDIR				:= $(current_dir)/lib
SDIR				:= $(current_dir)/src
PDIR				:= $(current_dir)/python
# DELETE after removing dependency on 'aux' headers
IDIR				+= $(current_dir)/include/aux

DICTSRCNAME	:= $(SDIR)/HAL_dict.cxx
ROOTIDIR		:= $(shell root-config --incdir)
PLATFORM		:= $(shell root-config --platform)

LIBS				:= -dynamiclib -Wl,-headerpad_max_install_names -single_module $(shell root-config --ldflags) $(shell root-config --evelibs)
ifeq ($(PLATFORM),macosx)
	LIBS			+= -Wl,-dead_strip_dylibs -Wl,-install_name,@rpath/$(LIBNAME)
endif

CXX					:= $(shell which `root-config --cxx`)
CXXFLAGS		:= $(shell root-config --cflags) $(addprefix -I,$(IDIR)) -I$(ROOTIDIR)
LINKER			:= $(shell which `root-config --ld`)

SRC					= $(wildcard $(SDIR)/*.cxx)
SRC					+= $(DICTSRCNAME)
OBJ					= $(addprefix $(ODIR)/,$(notdir $(addsuffix .o,$(basename $(SRC)))))

.PHONY: all install clean

.DEFAULT: all

all: $(DICTSRCNAME) $(LDIR)/libHAL.so

install: all
ifneq ($(LDIR),$(INSTALL_DIR))
	@cp $(LDIR)/$(LIBNAME) $(INSTALL_DIR)/. && \
	cp $(LDIR)/HAL_dict_rdict.pcm $(INSTALL_DIR)/. && \
	cp $(LDIR)/libHAL_dict.rootmap $(INSTALL_DIR)/.
	@echo 'Library and ROOT dictionary files installed in '$(INSTALL_DIR)
else
	@echo 'Library and ROOT dictionary files already installed in '$(INSTALL_DIR)
endif
ifneq ($(PDIR),$(INSTALL_DIR))
	@cp $(PDIR)/HAL.py $(INSTALL_DIR)/. && \
	cp $(PDIR)/HAL-config.py $(INSTALL_DIR)/.
	@echo 'Python files installed in '$(INSTALL_DIR)
else
	@echo 'Python files already installed in '$(INSTALL_DIR)
endif
	@python $(INSTALL_DIR)/HAL-config.py -w $(INSTALL_DIR) $(LDIR) $(IDIR)

# change directory
# call rootcling with output file
# 										includes
# 										where to put auxiliary files
# 										header that links to all other relevant headers
# 										special LinkDef file
$(DICTSRCNAME): $(current_dir)/include/HAL.h $(wildcard $(current_dir)/include/HAL/*.h) $(wildcard $(current_dir)/include/HAL/*.icc)
	@printf 'Generating ROOT dictionary %s ... ' $@ && \
	cd $(current_dir)/include && \
	rootcling -f $@ \
	-I$(ROOTIDIR) $(addprefix -I,$(IDIR)) \
	-s $(LDIR)/HAL_dict -rmf $(LDIR)/libHAL_dict.rootmap \
	$(current_dir)/include/HAL.h \
	$(current_dir)/include/HAL/HAL_LinkDef.h && \
	echo Done

# call linker to actually construct library
$(LDIR)/$(LIBNAME): $(OBJ)
	@printf 'Linking library %s ... ' $@ && \
	cd $(ODIR) && \
	$(LINKER) -o $@ $^ $(LIBS) && \
	echo Done

# build object files
# for clarificaiton see
# https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html
$(ODIR)/%.o: $(SDIR)/%.cxx
	@printf 'Compiling %s ... ' $< && \
	$(CXX) -c -o $@ $< $(CXXFLAGS) && \
	echo Done

# remove the object files and the library
clean:
	@printf 'Clearing dictionary, object, library, and Python files in ... ' && \
	rm -f $(DICTSRCNAME) $(OBJ) $(LDIR)/* $(INSTALL_DIR)/$(LIBNAME) && \
	echo Done
