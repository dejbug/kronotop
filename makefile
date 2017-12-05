define COMPILE
$(CXX) -o $1 -c $(filter %.cpp,$2) $(CXXFLAGS)
endef

define COPY_FILE
$(shell python -c "import shutil; shutil.copy('$1','$2')")
endef

define DISTINCT
$(sort $1)
endef

define GEN_PREREQ
$(CXX) -MF $1 -MM $2 -MT "$(subst .d,.o,$1) $1" $(CXXFLAGS)
endef

define HAS_GOAL
$(findstring $1,$(MAKECMDGOALS))
endef

define HAS_NON_BUILD_GOAL
$(or $(call HAS_GOAL,clean),$(call HAS_GOAL,reset))
endef

define LINK
$(CXX) -o $1 $(filter %.o %.a,$2) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
endef

define LIST_DIR
$(call TO_LIN_PATH,$(shell python -c "import os.path,itertools; it = os.walk('$1'); it = (tuple(os.path.join(t,n) for n in nn) for t,dd,nn in it); it = itertools.chain(*it); print ' '.join(it)"))
endef

define MAKE_DIR
$(shell python -c "import os.path; os.path.exists('$1') or os.makedirs('$1')")
endef

define REMOVE_TREE
$(shell python -c "import shutil; shutil.rmtree('$1', True)")
endef

define STRIP_PATH
$(call TO_LIN_PATH,$(shell python -c "print '$1'.strip(' /\\')"))
endef

define TO_ABS_PATH
$(call TO_LIN_PATH,$(shell python -c "import os.path; print os.path.abspath('$1')"))
endef

define TO_LIN_PATH
$(subst \,/,$1)
endef

define TO_WIN_PATH
$(subst /,\,$1)
endef

PRECISE := 1
DEBUG := 1

NAME := kronotope
TARGET := console
SOURCE_ROOT := src

INCDIRS := src extern/Scintilla/include
SYMBOLS := WIN32_LEAN_AND_MEAN STRICT
WINLIBS := gdi32

CXXFLAGS := --std=c++11 -fabi-version=11 -Wall -Wpedantic
CXXFLAGS +=	$(addprefix -I,$(INCDIRS))
CXXFLAGS +=	$(addprefix -D,$(SYMBOLS))

LDFLAGS :=
ifeq ($(TARGET),windows)
LDFLAGS += -Wl,--subsystem=windows
DEBUG := 0
else
LDFLAGS += -Wl,--subsystem=console
endif

ifneq ($(DEBUG),0)
CXXFLAGS += -g -Og
else
CXXFLAGS += -O3 -DNDEBUG
endif

LDLIBS := $(addprefix -l,$(WINLIBS))

SOURCES := $(call LIST_DIR,$(SOURCE_ROOT))
SOURCES := $(filter %.cpp,$(SOURCES))
PREREQS := $(SOURCES:src/%.cpp=build/%.d)
OBJECTS := $(SOURCES:src/%.cpp=build/%.o)
SUBDIRS := $(call DISTINCT,$(dir $(OBJECTS)))/

deploy/$(NAME).exe : deploy/SciLexer.dll
deploy/$(NAME).exe : build/resource.o
deploy/$(NAME).exe : $(OBJECTS) | deploy/ ; $(call LINK,$@,$^)

deploy/ build/ : ; $(call MAKE_DIR,$(call STRIP_PATH,$@))
$(SUBDIRS) : ; $(call MAKE_DIR,$@)

build/%.o : src/%.cpp | $(SUBDIRS) ; $(call COMPILE,$@,$^)
build/%.d : src/%.cpp | $(SUBDIRS) ; $(call GEN_PREREQ,$@,$<)

build/resource.o: src/main/main.rc | $(SUBDIRS) ; windres $< $@

deploy/SciLexer.dll : | $(SUBDIRS) ; $(call COPY_FILE,extern/Scintilla/bin/SciLexer.dll,deploy/)

.PHONY : clean reset run

clean : ; $(call REMOVE_TREE,build/)
reset : | clean ; $(call REMOVE_TREE,deploy/)
run : deploy/$(NAME).exe ; $<

ifneq ($(PRECISE),0)
ifeq (,$(call HAS_NON_BUILD_GOAL))
include $(PREREQS)
endif
endif

.DELETE_ON_ERROR :
