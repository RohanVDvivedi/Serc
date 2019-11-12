INC_DIR=./inc
OBJ_DIR=./obj
LIB_DIR=./lib
SRC_DIR=./src
BIN_DIR=./bin

CC=gcc
RM=rm -f

BOOTSTRAPFILES=bootstrapfiles

TARGET=app.out

CFLAGS=-I${INC_DIR} -I${INC_DIR}/${BOOTSTRAPFILES} -I${CONNMAN_PATH}/inc -I${BOOMPAR_PATH}/inc -I${CUTLERY_PATH}/inc

CUTLERY_LIB=-L${CUTLERY_PATH}/bin -lcutlery
BOOMPAR_LIB=-L${BOOMPAR_PATH}/bin -lboompar
CONNMAN_LIB=-L${CONNMAN_PATH}/bin -lconnman

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h
	${CC} ${CFLAGS} -c $< -o $@

BOOTSTRAP_SOURCES=${OBJ_DIR}/${BOOTSTRAPFILES}/distributer.o ${OBJ_DIR}/${BOOTSTRAPFILES}/responseStrings.o ${OBJ_DIR}/${BOOTSTRAPFILES}/serve.o ${OBJ_DIR}/${BOOTSTRAPFILES}/strhsh.o ${OBJ_DIR}/${BOOTSTRAPFILES}/httpobject.o ${OBJ_DIR}/${BOOTSTRAPFILES}/file_request_resolution_controller.o

${BIN_DIR}/$(TARGET) : ${BOOTSTRAP_SOURCES} ${OBJ_DIR}/controllers/first_controller.o
	gcc -o $@ src/main.c $(OBJ_DIR)/${BOOTSTRAPFILES}/*.o ${OBJ_DIR}/controllers/*.o ${CFLAGS} ${CONNMAN_LIB} ${BOOMPAR_LIB} ${CUTLERY_LIB}

route :
	python3 pyt/route.py routing routing1

all : route ${BIN_DIR}/$(TARGET)

run : route all

clean :
	$(RM) $(BIN_DIR)/*.out $(OBJ_DIR)/*.o $(OBJ_DIR)/${BOOTSTRAPFILES}/*.o