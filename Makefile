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

CUTLERY_LIB=-L${CUTLERY_PATH}/bin -lcutlery
BOOMPAR_LIB=-L${BOOMPAR_PATH}/bin -lboompar
CONNMAN_LIB=-L${CONNMAN_PATH}/bin -lconnman
JSON_PARSER_LIB=-L${JSON_PARSER_PATH}/bin -ljsonpar
RWLOCK_LIB=-L${RWLOCK_PATH}/bin -lrwlock

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h
	${CC} ${CFLAGS} -c $< -o $@

SERC_SOURCES=${OBJ_DIR}/http_server.o ${OBJ_DIR}/distributer.o ${OBJ_DIR}/http_object.o ${OBJ_DIR}/http_request.o ${OBJ_DIR}/http_response.o ${OBJ_DIR}/http_status.o ${OBJ_DIR}/http_method.o ${OBJ_DIR}/file_request_resolution_controller.o ${OBJ_DIR}/file_handler.o ${OBJ_DIR}/strhsh.o

${BIN_DIR}/$(TARGET) : ${SERC_SOURCES}
	ar rcs $@ $(OBJ_DIR)/*.o ${CFLAGS} ${CONNMAN_LIB} ${BOOMPAR_LIB} ${JSON_PARSER_LIB} ${CUTLERY_LIB} ${RWLOCK_LIB}

route :
	python3 pyt/route.py routing routing1

all : route ${BIN_DIR}/$(TARGET)

run : route all

clean :
	$(RM) $(BIN_DIR)/*.out $(OBJ_DIR)/*.o