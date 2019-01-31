CXXFLAGS = -std=c++17 -Wall -Werror

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0 -p
else
	CXXFLAGS += -O3
endif

all: build

build: baseline

run: baseline
	./baseline

baseline: parser.cpp parser.h main.cpp
	$(CXX) $(CXXFLAGS) -I. parser.cpp main.cpp \
	-L./lib -lxgboost -o baseline

clean:
	rm -f baseline compute_features
