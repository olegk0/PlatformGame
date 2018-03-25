CXXFLAGS =	-O0 -g -Wall -fmessage-length=0

OBJS =		my_game.o Disp.o EObject.o Sprite.o GMap.o MUser.o MSolidAdd.o

LIBS = -lX11 -lSDL -lpthread -lrt -DDEBUG

TARGET =	my_game

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
