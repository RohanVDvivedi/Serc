INC_DIR=./inc
OBJ_DIR=./obj
LIB_DIR=./lib
SRC_DIR=./src
BIN_DIR=./bin

CC=gcc
RM=rm -f

BOOTSTRAPFILES=bootstrapfiles

TARGET=libserc.a

CFLAGS=-I${INC_DIR} -I${CONNMAN_PATH}/inc -I${BOOMPAR_PATH}/inc -I${JSON_PARSER_PATH}/inc -I${CUTLERY_PATH}/inc -I${RWLOCK_PATH}/inc
LFLAFS=-L${CUTLERY_PATH}/bin -lcutlery -L${BOOMPAR_PATH}/bin -lboompar -L${CONNMAN_PATH}/bin -lconnman -L${JSON_PARSER_PATH}/bin -ljsonpar -L${RWLOCK_PATH}/bin -lrwlock

SERC_SOURCES=${OBJ_DIR}/http_server.o ${OBJ_DIR}/distributer.o ${OBJ_DIR}/http_object.o ${OBJ_DIR}/http_request.o ${OBJ_DIR}/http_response.o ${OBJ_DIR}/http_status.o ${OBJ_DIR}/http_method.o ${OBJ_DIR}/file_request_resolution_controller.o ${OBJ_DIR}/file_handler.o ${OBJ_DIR}/strhsh.o

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h ${CONNMAN_PATH}/inc ${BOOMPAR_PATH}/inc ${JSON_PARSER_PATH}/inc ${CUTLERY_PATH}/inc ${RWLOCK_PATH}/inc
	${CC} ${CFLAGS} -c $< -o $@

${BIN_DIR}/$(TARGET) : ${SERC_SOURCES}
	ar rcs $@ $(OBJ_DIR)/*.o ${CFLAGS}

route :
	python3 pyt/route.py routing routing1

all : route ${BIN_DIR}/$(TARGET)

run : route all

clean :
	$(RM) $(BIN_DIR)/*.out $(OBJ_DIR)/*.o