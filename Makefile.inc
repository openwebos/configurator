LIBS := -llunaservice -lmojoluna -lmojodb -lmojocore -lpmloglib $(LIBS)
LOCAL_LDFLAGS := $(LDFLAGS) $(LIBS)

GLIBCURL_SOURCES := glibcurl.c

CONFIGURATOR_SOURCES := \
        Log.cpp
	ActivityConfigurator.cpp \
	Configurator.cpp \
	DbKindConfigurator.cpp \
	DbPermissionsConfigurator.cpp \
	FileCacheConfigurator.cpp
		
CONFIGURATOR_MAIN := BusClient.cpp 
		
SOURCE_DIRS := src

CONFIGURATOR_TARGET := $(OBJDIR)/configurator
CONFIGURATOR_OBJECTS := $(CONFIGURATOR_SOURCES:%.cpp=$(OBJDIR)/%.o)
CONFIGURATOR_OBJECTS_MAIN := $(CONFIGURATOR_MAIN:%.cpp=$(OBJDIR)/%.o)

all: setup $(CONFIGURATOR_TARGET)
		
$(CONFIGURATOR_TARGET): $(CONFIGURATOR_OBJECTS) $(CONFIGURATOR_OBJECTS_MAIN)
	$(CXX) -o $(CONFIGURATOR_TARGET) $(CONFIGURATOR_OBJECTS) $(CONFIGURATOR_OBJECTS_MAIN) $(LOCAL_LDFLAGS) 
	
$(OBJDIR)/%.o: %.cpp
	$(CXX) -MMD $(INCLUDES) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $< -o $@

setup: $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

vpath %.cpp $(SOURCE_DIRS)
vpath %.c $(SOURCE_DIRS)

docs:
	(cd docs && doxygen ./Doxyfile)
	
clean:
	rm -rf $(OBJDIR)

SOURCES := $(CONFIGURATOR_SOURCES) $(CONFIGURATOR_MAIN)
-include $(SOURCES:%.cpp=$(OBJDIR)/%.d)

.PHONY: clean all setup
