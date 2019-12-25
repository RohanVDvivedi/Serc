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

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h ${CON_DIR}/config.h ${CONNMAN_PATH}/inc ${BOOMPAR_PATH}/inc ${JSON_PARSER_PATH}/inc ${CUTLERY_PATH}/inc ${RWLOCK_PATH}/inc
	${CC} ${CFLAGS} -c $< -o $@

${BIN_DIR}/$(TARGET) : ${SERC_SOURCES}
	ar rcs $@ $(OBJ_DIR)/*.o

route :
	# logic to list and pass as input parameter all .con files from CON_DIR
	python3 pyt/route.py ${CON_DIR}/routing.con

all : route ${BIN_DIR}/$(TARGET)

run : route all

clean :
	$(RM) $(BIN_DIR)/*.out $(OBJ_DIR)/*.o