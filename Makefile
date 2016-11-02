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
	LIBS += -Wl,-dead_strip_dylibs -Wl,-install_name,@rpath/$(LIBNAME)
endif
# cd /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib ; \
# clang++ -c  -m64 -pipe -W -Woverloaded-virtual -fsigned-char -fno-common -Qunused-arguments -pthread -std=c++14 -stdlib=libc++ -I/Users/jhetherly/src/root/build/6_06_08/release/include -I/Users/jhetherly/src/boost_1_58_0/build -I/Users/jhetherly/ProjectRepositories/HAL-ROOT/include -I/Users/jhetherly/ProjectRepositories/HAL-ROOT/include/aux -I"/Users/jhetherly/src/root/build/6_06_08/release/etc" -I"/Users/jhetherly/src/root/build/6_06_08/release/include" -I"/opt/local/libexec/llvm-3.7/bin/../include/c++/v1" -I"/Users/jhetherly/src/root/src/ROOT_REPO/interpreter/cling/include" -I"/Users/jhetherly/src/root/build/6_06_08/release/interpreter/cling/include" -I"/Users/jhetherly/src/root/build/6_06_08/release/etc/cling" -I"/Users/jhetherly/src/root/src/ROOT_REPO" -I"/Users/jhetherly/src/root/src/ROOT_REPO/graf3d/g3d/inc" -I"/Users/jhetherly/src/root/src/ROOT_REPO/gui/gui/inc" -I"/Users/jhetherly/src/root/src/ROOT_REPO/io/io/inc" -I"/Users/jhetherly/src/root/src/ROOT_REPO/core/base/../textinput/src" -I"/Users/jhetherly/src/root/build/6_06_08/release/core/base/" -I"/Users/jhetherly/src/root/build/6_06_08/release/core/rint/" -I"/Users/jhetherly/src/root/build/6_06_08/release/core/thread/" -I"/Users/jhetherly/src/root/build/6_06_08/release/io/io/" -I"/Users/jhetherly/src/root/src/ROOT_REPO/hist/hist/inc" -I"/Users/jhetherly/src/root/build/6_06_08/release/math/mathcore/" /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Algorithm.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Analysis.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AnalysisData.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AnalysisSelector.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AnalysisTreeReader.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AnalysisTreeWriter.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AnalysisUtils.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/AttachAttribute.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Cut.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/CutAlgorithm.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/CutOptimizer.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/GenericData.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/GenericParticle.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/HAL_dict.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/ImportParticle.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/ImportValue.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Integrator.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Interpolator.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/Monitor.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/PlotUtils.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/SelectParticle.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/SelectRank.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/SelectRefParticle.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/StoreParticle.cxx /Users/jhetherly/ProjectRepositories/HAL-ROOT/src/VecAddReco.cxx  ; \
# clang++ /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Algorithm.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Analysis.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AnalysisData.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AnalysisSelector.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AnalysisTreeReader.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AnalysisTreeWriter.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AnalysisUtils.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/AttachAttribute.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Cut.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/CutAlgorithm.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/CutOptimizer.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/GenericData.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/GenericParticle.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/HAL_dict.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/ImportParticle.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/ImportValue.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Integrator.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Interpolator.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/Monitor.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/PlotUtils.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/SelectParticle.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/SelectRank.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/SelectRefParticle.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/StoreParticle.o /Users/jhetherly/ProjectRepositories/HAL-ROOT/lib/VecAddReco.o  -dynamiclib -Wl,-headerpad_max_install_names -m64 -single_module -Wl,-dead_strip_dylibs  -L/Users/jhetherly/src/root/build/6_06_08/release/lib -lGui -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lpthread -stdlib=libc++ -lm -ldl -lTreePlayer  -o libHAL.so -Wl,-install_name,@rpath/libHAL.so

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
	@cp $(PDIR)/HAL.py $(INSTALL_DIR)/.
	@echo 'Python files installed in '$(INSTALL_DIR)
else
	@echo 'Python files already installed in '$(INSTALL_DIR)
endif

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
	echo 'Done'
