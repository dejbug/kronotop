
NAME := skeletype
# TARGET := windows
TARGET := console
SHELL := cmd.exe

WP = $(subst /,\,$1)

MKDIR = IF NOT EXIST $(call WP,$1) mkdir $(call WP,$1)
COPY = copy $(call WP,$1) $(call WP,$2)
DEL = IF EXIST $(call WP,$1) del $(call WP,$1) 2>NUL
DELTREE = IF EXIST $(call WP,$1) rmdir /S /Q $(call WP,$1)

CXX := g++

CXXFLAGS :=
CXXFLAGS +=	--std=c++14 -Wpedantic -Wall -Os
CXXFLAGS +=	-DWIN32_LEAN_AND_MEAN
CXXFLAGS +=	-DSTRICT
CXXFLAGS +=	-Wl,--subsystem=$(TARGET)
CXXFLAGS +=	-I./src
CXXFLAGS +=	-I./extern/Scintilla/include
CXXFLAGS +=	-lgdi32
# CXXFLAGS +=	-lgdiplus -lcomctl32
# CXXFLAGS +=	-lshlwapi -lntdll
# CXXFLAGS +=	-DDBG_PRINT_SCI_MESSAGES

# -- since make doesn't support wildcards on subdirs.
# listdir = $(shell dir /S /B $(1)*$(2))

build/$(NAME).exe: build_dir_tree

# build/$(NAME).exe: build/Scintilla.dll
build/$(NAME).exe: build/SciLexer.dll

build/$(NAME).exe: build/dejlib2.a
build/$(NAME).exe: build/dejlib3.a

build/$(NAME).exe: build/resource.o
build/$(NAME).exe: build/WinMain.o
build/$(NAME).exe: build/MainFrameProc.o
build/$(NAME).exe: build/App.o

build/$(NAME).exe: ; $(CXX) $(filter %.o,$^) $(filter %.a,$^) -o $@ $(CXXFLAGS)


.PHONY: build_dir_tree
build_dir_tree:
	$(call MKDIR,build/dejlib2)
	$(call MKDIR,build/dejlib3)


build/Scintilla.dll: ; $(call COPY,extern/Scintilla/bin/Scintilla.dll,build/.)

build/SciLexer.dll: ; $(call COPY,extern/Scintilla/bin/SciLexer.dll,build/.)

build/dejlib2.a: build/dejlib2/dejlib.o
build/dejlib2.a: ; @ar r $@ $(filter %.o,$^)

build/dejlib2/dejlib.o: src/dejlib2/dejlib.cpp
build/dejlib2/dejlib.o: src/dejlib2/*.h

build/dejlib2/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)

build/dejlib3.a: build/dejlib3/win.o
build/dejlib3.a: build/dejlib3/sci.o
build/dejlib3.a: build/dejlib3/dbg.o
build/dejlib3.a: build/dejlib3/scinti.o
build/dejlib3.a: ; @ar r $@ $(filter %.o,$^)

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

build/dejlib3/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)

build/resource.o: src/main/main.rc ; @windres $< $@

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

build/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)
# build/dejlib2/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)
# build/dejlib3/%.o: ; $(CXX) $(filter %.cpp,$^) -o $@ -c $(CXXFLAGS)

.PHONY: clean
clean:
	$(call DEL,build/$(NAME).exe)
	$(call DEL,build/*.o)
	$(call DEL,build/dejlib2/*.o)
	$(call DEL,build/dejlib3/*.o)

.PHONY: reallyclean
reallyclean:
	$(call DELTREE,"build")

.PHONY: run
run: build/$(NAME).exe
	@build\$(NAME).exe
