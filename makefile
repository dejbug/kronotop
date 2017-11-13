SHELL := cmd.exe
TARGET ?= console
WINLIBS := gdi32
# WINLIBS += gdiplus comctl32 shlwapi ntdll
INCDIRS := src extern/Scintilla/include
SYMBOLS := WIN32_LEAN_AND_MEAN STRICT
# SYMBOLS += DBG_PRINT_SCI_MESSAGES
CXX := g++

CXXFLAGS := --std=c++11 -fabi-version=11 -Wall -Wpedantic -O2
CXXFLAGS +=	$(addprefix -I,$(INCDIRS))
CXXFLAGS +=	$(addprefix -D,$(SYMBOLS))

LDFLAGS := -Wl,-subsystem=$(TARGET)
LDLIBS := $(addprefix -l,$(WINLIBS))

NAME := skeletype

WP = $(subst /,\,$1)
MKDIR = IF NOT EXIST $(call WP,$1) mkdir $(call WP,$1)
COPY = copy $(call WP,$1) $(call WP,$2)
DEL = IF EXIST $(call WP,$1) del $(call WP,$1) 2>NUL
DELTREE = IF EXIST $(call WP,$1) rmdir /S /Q $(call WP,$1)

# -- since make doesn't support wildcards on subdirs.
# listdir = $(shell dir /S /B $(1)*$(2))

deploy/$(NAME).exe: build_dir_tree
deploy/$(NAME).exe: build/resource.o
deploy/$(NAME).exe: build/WinMain.o
deploy/$(NAME).exe: build/MainFrameProc.o
deploy/$(NAME).exe: build/App.o
deploy/$(NAME).exe: build/dejlib2.a
deploy/$(NAME).exe: build/dejlib3.a
# deploy/$(NAME).exe: deploy/Scintilla.dll
deploy/$(NAME).exe: deploy/SciLexer.dll
deploy/$(NAME).exe:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.o %.a,$^) $(LDLIBS)

.PHONY: build_dir_tree
build_dir_tree:
	$(call MKDIR,build/dejlib2)
	$(call MKDIR,build/dejlib3)
	$(call MKDIR,deploy)

deploy/Scintilla.dll:
	$(call COPY,extern/Scintilla/bin/Scintilla.dll,deploy/.)
deploy/SciLexer.dll:
	$(call COPY,extern/Scintilla/bin/SciLexer.dll,deploy/.)

build/dejlib2.a: build/dejlib2/dejlib.o
build/dejlib2.a: ; ar r $@ $(filter %.o,$^)

build/dejlib2/dejlib.o: src/dejlib2/dejlib.cpp
build/dejlib2/dejlib.o: src/dejlib2/*.h

build/dejlib2/%.o: ; $(CXX) $(CXXFLAGS) -o $@ -c $(filter %.cpp,$^)

build/dejlib3.a: build/dejlib3/win.o
build/dejlib3.a: build/dejlib3/sci.o
build/dejlib3.a: build/dejlib3/dbg.o
build/dejlib3.a: build/dejlib3/scinti.o
build/dejlib3.a: ; ar r $@ $(filter %.o,$^)

build/dejlib3/win.o: src/dejlib3/win.cpp
build/dejlib3/win.o: src/dejlib3/win.h
build/dejlib3/sci.o: src/dejlib3/sci.cpp
build/dejlib3/sci.o: src/dejlib3/sci.h
build/dejlib3/dbg.o: src/dejlib3/dbg.cpp
build/dejlib3/dbg.o: src/dejlib3/dbg.h
build/dejlib3/scinti.o: src/dejlib3/scinti.cpp
build/dejlib3/scinti.o: src/dejlib3/scinti.h

build/dejlib3/sci.o: src/dejlib3/dbg.h
src/dejlib3/scinti.o: src/dejlib3/dbg.h

build/dejlib3/%.o: ; $(CXX) $(CXXFLAGS) -o $@ -c $(filter %.cpp,$^)

build/resource.o: src/main/main.rc ; windres $< $@

build/WinMain.o: src/main/WinMain.cpp
build/WinMain.o: build/dejlib2.a
build/WinMain.o: build/MainFrameProc.o

build/MainFrameProc.o: build/dejlib2.a
build/MainFrameProc.o: build/dejlib3.a
build/MainFrameProc.o: build/App.o
build/MainFrameProc.o: src/main/MainFrameProc.cpp
build/MainFrameProc.o: src/snippets/*.h

build/App.o: src/main/App.cpp
build/App.o: build/dejlib3.a

src/main/App.cpp: src/main/App.h

build/%.o: ; $(CXX) $(CXXFLAGS) -o $@ -c $(filter %.cpp,$^)
# build/dejlib2/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)
# build/dejlib3/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)

.PHONY: clean
clean:
	$(call DEL,build/*.o)
	$(call DELTREE,build/dejlib2)
	$(call DELTREE,build/dejlib3)
	$(call DELTREE,"deploy")

.PHONY: reset
reset:
	$(call DELTREE,"build")
	$(call DELTREE,"deploy")

.PHONY: run
run: deploy/$(NAME).exe
	$<
