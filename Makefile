INC_DIR=./inc
OBJ_DIR=./obj
LIB_DIR=./lib
SRC_DIR=./src
BIN_DIR=./bin

# this is the config directory, where we have to store, routing and server root directory configurations
CON_DIR=./con

CC=gcc
RM=rm -f

TARGET=libserc.a

CFLAGS=-I${INC_DIR} -I${CON_DIR} -I${CONNMAN_PATH}/inc -I${BOOMPAR_PATH}/inc -I${JSON_PARSER_PATH}/inc -I${CUTLERY_PATH}/inc -I${RWLOCK_PATH}/inc
LFLAFS=-L${CUTLERY_PATH}/bin -lcutlery -L${BOOMPAR_PATH}/bin -lboompar -L${CONNMAN_PATH}/bin -lconnman -L${JSON_PARSER_PATH}/bin -ljsonpar -L${RWLOCK_PATH}/bin -lrwlock

SERC_SOURCES=${OBJ_DIR}/http_server.o ${OBJ_DIR}/distributer.o ${OBJ_DIR}/http_object.o ${OBJ_DIR}/http_request.o ${OBJ_DIR}/http_response.o ${OBJ_DIR}/http_status.o ${OBJ_DIR}/http_method.o ${OBJ_DIR}/file_request_resolution_controller.o ${OBJ_DIR}/file_handler.o ${OBJ_DIR}/strhsh.o

${SRC_DIR}/distributer.c : ${CON_DIR}/*.con
	python3 pyt/route.py $^

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h ${CON_DIR}/config.h ${CONNMAN_PATH}/inc ${BOOMPAR_PATH}/inc ${JSON_PARSER_PATH}/inc ${CUTLERY_PATH}/inc ${RWLOCK_PATH}/inc
	${CC} ${CFLAGS} -c $< -o $@

${BIN_DIR}/$(TARGET) : ${SERC_SOURCES}
	ar rcs $@ $(OBJ_DIR)/*.o

all : ${BIN_DIR}/$(TARGET)

# you may use the below command to make a executable that starts the server to host a static website
server :
	gcc -o ${BIN_DIR}/server.out main.c ${CFLAGS} ${LFLAFS} -L${BIN_DIR} -lserc

run : route all

clean :
	$(RM) $(BIN_DIR)/*.out $(OBJ_DIR)/*.o