PRJ = ad_blinky

CFLAGS = -lrt -lpthread #-Wall -Wshadow
SRC = arduPi.cpp main.cpp #ResPi_Test.cpp# main.cpp


all:
	sudo g++ $(CFLAGS) $(SRC) -o $(PRJ)

.PHONY : clean
clean:
	sudo rm $(PRJ)
