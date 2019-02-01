CXXFLAGS = -std=c++17 -Wall -Werror
XGB_PATH := $(shell bash get_xgboost_path.sh)


ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0 -p -fsanitize=address
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
	rm -f baseline
