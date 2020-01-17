CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -DDEBUG
LDFLAGS =
RM = gio trash -f


COMMON_HDR = Exception.h
TARGETS =  ann-result-writer-test timer-test string-utils-test filename-utils-test benchEigen bitop bvecs-reader-demo fvecs-reader-demo
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)


all: $(TARGETS)

ann-result-writer-test: AnnResultWriterTest.o AnnResultWriter.hpp $(COMMON_HDR)
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

timer-test: TimerTest.o Timer.hpp
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

string-utils-test: StringUtilsTest.o StringUtils.hpp $(COMMON_HDR)
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

filename-utils-test: FilenameUtilsTest.o FilenameUtils.hpp StringUtils.hpp $(COMMON_HDR)
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

benchEigen: benchEigen.o
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

bitop: bitop.o
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

bvecs-reader-demo: BvecsReaderDemo.o
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

fvecs-reader-demo: FvecsReaderDemo.o
	-$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	-$(RM) $(OBJS) $(TARGETS) AnnResultWriterTest TimerTest
