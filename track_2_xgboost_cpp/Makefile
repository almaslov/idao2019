CXXFLAGS = -std=c++17 -Wall -Werror
# XGB_PATH=/usr/conda/lib/python3.6/site-packages/xgboost
# XGB_PATH=/venv/lib/python3.5/site-packages/xgboost
# XGB_PATH := $(shell bash get_xgboost_path.sh)



ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0 -p -fsanitize=address
else
	CXXFLAGS += -O3
endif

all: build

build: baseline

run: baseline
	./baseline

generated.h: generate_solution.py
	python3 generate_solution.py >generated.h

local: feature_functions.h generated.h main.cpp
	cat feature_functions.h generated.h main.cpp >generated_main.cpp && \
	./create_archive.sh Makefile parser.h parser.cpp include lib my.xgb generated_main.cpp

baseline: parser.cpp parser.h generated_main.cpp
	$(CXX) $(CXXFLAGS) \
	-I./include \
	parser.cpp generated_main.cpp \
	-L./lib -lxgboost \
	-Wl,-rpath,./lib \
	-o baseline && \
	./baseline --model-info

clean:
	ls -al `find . -type f` && rm -f baseline
