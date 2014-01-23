CXX=g++
RM=rm -f

GMOCK_DIR=thirdparty/gmock
GTEST_DIR=$(GMOCK_DIR)/gtest

GMOCK_INC=$(GMOCK_DIR)/include
GTEST_INC=$(GTEST_DIR)/include
GMOCK_SRC=$(GMOCK_DIR)/src/gmock-all.cc
GTEST_SRC=$(GTEST_DIR)/src/gtest-all.cc

COMMONFLAGS=-Wall -Werror -std=c++0x
DEBUGFLAGS=-g -O0 -fno-inline -DGEO_DEBUG
RELEASEFLAGS=-Os

SRCS=$(wildcard test/*.cpp) $(GMOCK_SRC) $(GTEST_SRC)
INC=geo $(GMOCK_INC) $(GTEST_INC) $(GMOCK_DIR) $(GTEST_DIR)
LIB=-lpthread

all: debug release

debug:
	$(CXX) $(SRCS) $(addprefix -I, $(INC)) $(LIB) $(COMMONFLAGS) $(DEBUGFLAGS) -o geo_test_dbg

release: 
	$(CXX) $(SRCS) $(addprefix -I, $(INC)) $(LIB) $(COMMONFLAGS) $(RELEASEFLAGS) -o geo_test_rel

run_debug: debug
	./geo_test_dbg

run_release: release
	./geo_test_rel

clean:
	$(RM) geo_test*
	$(MAKE) -C thirdparty/gmock/make clean
