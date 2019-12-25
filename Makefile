SERC_ROOT=.

INC_DIR=${SERC_ROOT}/inc
OBJ_DIR=${SERC_ROOT}/obj
LIB_DIR=${SERC_ROOT}/lib
SRC_DIR=${SERC_ROOT}/src
BIN_DIR=${SERC_ROOT}/bin

# this is the config directory, 
# where we must store routing and server root directory configurations
CON_DIR=${SERC_ROOT}/con

CC=gcc
RM=rm -f

TARGET=libserc.a

CFLAGS=-I${INC_DIR} -I${CON_DIR} -I${CONNMAN_PATH}/inc -I${BOOMPAR_PATH}/inc -I${JSON_PARSER_PATH}/inc -I${CUTLERY_PATH}/inc -I${RWLOCK_PATH}/inc
LFLAFS=-L${CUTLERY_PATH}/bin -lcutlery -L${BOOMPAR_PATH}/bin -lboompar -L${CONNMAN_PATH}/bin -lconnman -L${JSON_PARSER_PATH}/bin -ljsonpar -L${RWLOCK_PATH}/bin -lrwlock

SOURCES:=${shell find $(SRC_DIR) -name '*.c'}
OBJECTS:=$(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,${SOURCES})

${SRC_DIR}/distributer.c ${INC_DIR}/controller.h : ${CON_DIR}/*.con
	python3 pyt/route.py $^

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h ${CON_DIR}/config.h ${CONNMAN_PATH}/inc ${BOOMPAR_PATH}/inc ${JSON_PARSER_PATH}/inc ${CUTLERY_PATH}/inc ${RWLOCK_PATH}/inc
	${CC} ${CFLAGS} -c $< -o $@

${BIN_DIR}/$(TARGET) : ${OBJECTS}
	ar rcs $@ $(OBJECTS)

all : ${BIN_DIR}/$(TARGET)

# you may use the below command to make a executable that starts the server to host a static website
server :
	gcc -o ${BIN_DIR}/server.out main.c ${CFLAGS} ${LFLAFS} -L${BIN_DIR} -lserc

clean :
	$(RM) $(BIN_DIR)/* $(OBJ_DIR)/*