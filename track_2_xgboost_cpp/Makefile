CXXFLAGS = -std=c++17 -Wall -Werror
XGB_PATH := $(shell python3 -c 'import xgboost; print(xgboost.__path__[0])')


ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0 -p
else
	CXXFLAGS += -O3
endif

all: build

build: baseline

run: baseline
	./baseline

baseline: parser.cpp parser.h main.cpp solution.h index.h feature.h
	$(CXX) $(CXXFLAGS) \
	-I$(XGB_PATH)/include \
	-I$(XGB_PATH)/rabit/include \
	parser.cpp main.cpp \
	-L$(XGB_PATH)/lib -lxgboost \
	-Wl,-rpath,$(XGB_PATH)/lib \
	-o baseline

clean:
	rm -f baseline compute_features
