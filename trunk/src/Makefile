PROGRAM=Parbat
CXX=wineg++
CXXFLAGS=-c -Wall -O2 -DWINE_NOWINSOCK -DNOCRYPT -DNOMINMAX
CXXFLAGS+=

LDFLAGS=
LDFLAGS+=-lcomdlg32 -lshell32 -lkernel32 -luser32 -ladvapi32 -lGL -lGLU -lgdal -luser32 -lcomctl32
CXXANDLDFLAGS=-ggdb -mwindows -Wno-deprecated

# Core files
SOURCES=BandInfo.cpp \
BatchIniFile.cpp \
console.cpp \
ContrastAdvWindow.cpp \
ContrastWindow.cpp \
CoordinateInfo.cpp \
DisplayTab.cpp \
DisplayWindow.cpp \
FeatureSpace.cpp \
FeatureSpaceGL.cpp \
FeatureTab.cpp \
GLContainer.cpp \
GLText.cpp \
GLView.cpp \
ImageFile.cpp \
ImageGL.cpp \
ImageHandler.cpp \
ImageProperties.cpp \
ImageTab.cpp \
ImageTileSet.cpp \
ImageViewport.cpp \
ImageWindow.cpp \
IniFile.cpp \
main.cpp \
MainWindow.cpp \
OverviewGL.cpp \
OverviewWindow.cpp \
PrefsWindow.cpp \
ProgressWindow.cpp \
QueryTab.cpp \
ROI.cpp \
ROIFile.cpp \
ROISet.cpp \
ROIWindow.cpp \
ScrollBox.cpp \
Settings.cpp \
StickyWindowManager.cpp \
StringUtils.cpp \
TestUtils.cpp \
ToolTab.cpp \
ToolWindow.cpp \
ViewportListener.cpp \
Window.cpp

# List of objects...
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) $(CXXANDLDFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp PchApp.h.gch
	$(CXX) $(CXXANDLDFLAGS) $(CXXFLAGS) $< -o $@

PchApp.h.gch: PchApp.h
	$(CXX) $(CXXANDLDFLAGS) $(CXXFLAGS) PchApp.h

scratch: distclean $(PROGRAM)

clean:
	@rm -f -v *.o
	@rm -f -v PchApp.h.gch

distclean: clean
	@rm -f -v $(PROGRAM)
	@rm -f -v $(PROGRAM)D
	@rm -f -v *.so

run: $(PROGRAM)
	$(RUN_ENVIRONMENT) ./$(PROGRAM) $(RUN_ARGUMENTS)

rund: $(PROGRAM)
	$(RUN_ENVIRONMENT) gdb $(PROGRAM) $(RUN_ARGUMENTS)
