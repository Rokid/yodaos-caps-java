.PHONY: all

.PHONY: caps-jar caps-jni

.PHONY: clean

all: caps-jar caps-jni

ROOT_DIR := $(shell pwd)
OUTBIN_DIR := $(ROOT_DIR)/out/bin
OUTLIB_DIR := $(ROOT_DIR)/out/lib
OUTINTER_DIR := $(ROOT_DIR)/out/intermediates
OUT_JAVACLASS_DIR := $(OUTINTER_DIR)/java-classes
OUT_OBJS_DIR := $(OUTINTER_DIR)/objs
JAVAC_FLAGS := -g:none

# ubuntu
CXX := g++
DL_SUFFIX := so
# set include dir of jni.h/caps.h
CAPS_JNI_INCLUDES := \
	/home/ming/bin/jdk1.8.0_151/include \
	/home/ming/bin/jdk1.8.0_151/include/linux \
	/home/ming/buildroot/host/usr/include/caps
# set dir of libcaps
CAPS_JNI_LIB_PATHS := \
	/home/ming/buildroot/host/usr/lib
ALT_CXXFLAGS := -Wno-write-strings
ALT_LDFLAGS := -shared -s
# macosx
# CXX := c++
# DL_SUFFIX := dylib
# CAPS_JNI_INCLUDES := \
	/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers \
	/Users/zhangchen/work/buildroot/host/usr/include/caps
# CAPS_JNI_LIB_PATHS := \
	/Users/zhangchen/work/buildroot/host/usr/lib
# ALT_CXXFLAGS := -Wno-writable-strings
# ALT_LDFLAGS := -dynamiclib

CAPS_JNI_DEP_LIBS := caps
CXXFLAGS := -fPIC -std=c++11 -O3 \
	$(addprefix -I, $(CAPS_JNI_INCLUDES)) \
	$(ALT_CXXFLAGS)
LDFLAGS := $(ALT_LDFLAGS) \
	$(addprefix -L, $(CAPS_JNI_LIB_PATHS)) \
	$(addprefix -l, $(CAPS_JNI_DEP_LIBS))

CAPS_JAVA_SOURCE_DIR := $(ROOT_DIR)/src/java
CAPS_JAVA_SOURCE_FILES_ABS := $(shell find $(CAPS_JAVA_SOURCE_DIR) -name *.java)
CAPS_JAVA_SOURCE_FILES := $(patsubst $(CAPS_JAVA_SOURCE_DIR)/%, %, $(CAPS_JAVA_SOURCE_FILES_ABS))
CAPS_JAVA_CLASSES := $(patsubst %.java, %.class, $(CAPS_JAVA_SOURCE_FILES))
CAPS_JAVA_CLASSES_ABS := $(addprefix $(OUT_JAVACLASS_DIR)/, $(CAPS_JAVA_CLASSES))
CAPS_JNI_SOURCE_DIR := $(ROOT_DIR)/src/jni
CAPS_JNI_OBJS_ABS :=
CAPS_JNI_OBJS :=
CAPS_JNI_SOURCE_FILES_ABS := $(shell find $(CAPS_JNI_SOURCE_DIR) -name *.cpp)
CAPS_JNI_OBJS_ABS := $(patsubst $(CAPS_JNI_SOURCE_DIR)/%.cpp, $(OUT_OBJS_DIR)/%.o, $(CAPS_JNI_SOURCE_FILES_ABS))

caps-jar: $(OUTBIN_DIR)/caps.jar

$(OUTBIN_DIR)/caps.jar: $(CAPS_JAVA_CLASSES_ABS)
	mkdir -p $(OUTBIN_DIR)
	jar cf $@ -C $(OUT_JAVACLASS_DIR) .

$(CAPS_JAVA_CLASSES_ABS): $(OUT_JAVACLASS_DIR)

$(OUT_JAVACLASS_DIR):
	mkdir -p $(OUT_JAVACLASS_DIR)

$(OUT_JAVACLASS_DIR)/%.class: $(CAPS_JAVA_SOURCE_DIR)/%.java
	javac $(JAVAC_FLAGS) -sourcepath $(CAPS_JAVA_SOURCE_DIR) -d $(OUT_JAVACLASS_DIR) $<

caps-jni: $(OUTLIB_DIR)/libcaps-jni.$(DL_SUFFIX)

$(OUTLIB_DIR)/libcaps-jni.$(DL_SUFFIX): $(CAPS_JNI_OBJS_ABS)
	mkdir -p $(OUTLIB_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(CAPS_JNI_OBJS_ABS): $(OUT_OBJS_DIR)

$(OUT_OBJS_DIR):
	mkdir -p $(OUT_OBJS_DIR)

$(OUT_OBJS_DIR)/%.o: $(CAPS_JNI_SOURCE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf out
