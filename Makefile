# run make to create the client and server executables 
CC = g++

SRC_DIR := src
OBJ_DIR := obj

EXE := client server
SRC := $(wildcard $(SRC_DIR)/*.cpp)
SERV_OBJ = $(OBJ_DIR)/serverapp.o $(OBJ_DIR)/netutils.o $(OBJ_DIR)/ClientInfo.o $(OBJ_DIR)/Server.o $(OBJ_DIR)/Packet.o
CLIENT_OBJ = $(OBJ_DIR)/clientapp.o $(OBJ_DIR)/netutils.o $(OBJ_DIR)/Client.o $(OBJ_DIR)/Packet.o $(OBJ_DIR)/Display.o

CPPFLAGS := -Iinclude
CFLAGS := -Wall
LDFLAGS := -Llib -pthread -lncurses
LDLIBS := -lm

.PHONY: all clean

# creates both the client and server executables
all: $(EXE)

client: $(CLIENT_OBJ)
	$(CC) $^ $(LDLIBS) -o $@ $(LDFLAGS) 

server: $(SERV_OBJ)
	$(CC) $^ $(LDLIBS) -o $@ $(LDFLAGS) 
	
displaytest: displaytest.o obj/Display.o
	$(CC) $^ $(LDLIBS) -o $@ $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean: 
	@$(RM) -rv $(OBJ_DIR) $(EXE) out.txt displaytest
