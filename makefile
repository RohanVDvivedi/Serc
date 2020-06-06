#list of all the directories, in the project
INC_DIR=./inc
OBJ_DIR=./obj
LIB_DIR=./lib
SRC_DIR=./src
BIN_DIR=./bin

# this is the config directory, 
# where we must store routing and server root directory configurations
CON_DIR=./con

CC=gcc
RM=rm -f

# figure out all the sources in the project
SOURCES:=${shell find $(SRC_DIR) -name '*.c'}
# and the required objects to be built
OBJECTS:=$(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,${SOURCES})
# the traget library
TARGET:=${BIN_DIR}/libserc.a

# place your include directories -I flag here
CFLAGS=-Wall -I${INC_DIR} -I${CON_DIR} -I${CONNMAN_PATH}/inc -I${BOOMPAR_PATH}/inc -I${JSON_PARSER_PATH}/inc -I${CUTLERY_PATH}/inc -I${RWLOCK_PATH}/inc

# the linker flags, here, which will help you compile the simple server
LFLAFS=-L${CONNMAN_PATH}/bin -lconnman -L${BOOMPAR_PATH}/bin -lpthread -lboompar -L${JSON_PARSER_PATH}/bin -ljsonpar -L${CUTLERY_PATH}/bin -lcutlery -L${RWLOCK_PATH}/bin -lrwlock -lz

# the header files from external libraries
HEADER_DEPENDENCIES=${CONNMAN_PATH}/inc/*.h ${BOOMPAR_PATH}/inc/*.h ${JSON_PARSER_PATH}/inc/*.h ${CUTLERY_PATH}/inc/*.h ${RWLOCK_PATH}/inc/*.h

# the routing is constructed by this dependency make statement
${SRC_DIR}/distributer.c ${INC_DIR}/controller.h : ${CON_DIR}/*.con
	python3 pyt/route.py $^

# rule to make the object directory
${OBJ_DIR} :
	mkdir -p $@

# generic rule to build any object file
${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h ${CON_DIR}/config.h ${HEADER_DEPENDENCIES} | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@

# rule to make the directory for binaries or libraries
${BIN_DIR} :
	mkdir -p $@

# generic rule to make a library target
${TARGET} : ${OBJECTS} | ${BIN_DIR}
	ar rcs $@ ${OBJECTS}

all : $(TARGET)

# this rule will re build all the routing of your application
routes : ${SRC_DIR}/distributer.c ${INC_DIR}/controller.h

# you may use the below command to make a executable that starts the server to host a static website
server :
	gcc -o ${BIN_DIR}/server.out server.c -L${BIN_DIR} -lserc ${CFLAGS} ${LFLAFS}

# you may use the below command to make a executable creates a single threaded threadpooled http transaction client to make a test request
client :
	gcc -o ${BIN_DIR}/client.out client.c -L${BIN_DIR} -lserc ${CFLAGS} ${LFLAFS}

clean :
	$(RM) -r ${BIN_DIR} ${OBJ_DIR}