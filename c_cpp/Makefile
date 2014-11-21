.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TARGET=train
TARGET2=test
all: $(TARGET) $(TARGET2)
# type make/make all to compile test_hmm
	g++ train.cpp -o train
	g++ test.cpp -o test

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file
