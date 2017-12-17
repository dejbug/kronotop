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
$(call TO_LIN_PATH,$(shell python -c "import os,itertools,urllib; it = os.walk('$1'); it = (tuple(os.path.join(t,n) for n in nn) for t,dd,nn in it); it = itertools.chain(*it); print ' '.join(it)"))
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

define QUOTE
$(addsuffix ",$(addprefix ",$1))
endef
