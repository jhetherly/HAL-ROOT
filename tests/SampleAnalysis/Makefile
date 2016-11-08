ANALYSIS_NAME := MyAnalysis

HAL_LDIR		:= $(shell python ../../lib/HAL-config.py --libdir)
HAL_IDIR		:= $(shell python ../../lib/HAL-config.py --incdir)
# DELETE after removing dependency on 'aux' headers
HAL_IDIR		+= $(HAL_IDIR)/aux

current_dir	:= $(CURDIR)
IDIR				:= $(current_dir)/algorithms/include
ODIR				:= $(current_dir)/bin
BDIR				:= $(current_dir)/bin
SDIR				:= $(current_dir)/analyses

ROOTIDIR		:= $(shell root-config --incdir)

LIBS				:= -Wl,-headerpad_max_install_names -single_module $(shell root-config --ldflags) $(shell root-config --evelibs)
# rpath ust be relative to wherever the executable is run (or an absolute path)
LIBS				+= -L$(HAL_LDIR) $(shell python ../../lib/HAL-config.py --libs) -rpath $(HAL_LDIR)

CXX					:= $(shell which `root-config --cxx`)
CXXFLAGS		:= $(shell root-config --cflags) $(addprefix -I,$(HAL_IDIR)) $(addprefix -I,$(IDIR)) -I$(ROOTIDIR)

SRC					= $(wildcard $(SDIR)/*.cxx)
OBJ					= $(addprefix $(ODIR)/,$(notdir $(addsuffix .o,$(basename $(SRC)))))

.PHONY: all clean

.DEFAULT: all

all: $(BDIR)/$(ANALYSIS_NAME)

# build object files
# for clarificaiton see
# https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html
$(ODIR)/%.o: $(SDIR)/%.cxx
	@printf 'Compiling %s ... ' $< && \
	$(CXX) -c -o $@ $< $(CXXFLAGS) && \
	echo Done

$(BDIR)/$(ANALYSIS_NAME): $(OBJ)
	@printf 'Linking library %s ... ' $@ && \
	cd $(ODIR) && \
	$(CXX) -o $@ $^ $(LIBS) && \
	echo Done

# remove the object files and the executable
clean:
	@printf 'Clearing object and library files in ... ' && \
	rm -f $(OBJ) $(BDIR)/$(ANALYSIS_NAME) && \
	echo Done
