CXX = g++
CXXFLAGS = -std=c++17 -Wall -Ivisual -Icore -Itool
LDFLAGS = -lssl -lcrypto

SRC = main.cpp \
      core/menu.cpp \
      tool/PASS/ghostkey.cpp \
      core/visual/banner.cpp \
      tool/WALLET/hidewallet.cpp \
			tool/LOCALSPACE/localspace.cpp

OBJ = $(SRC:.cpp=.o)

TARGET = ghostkey

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

