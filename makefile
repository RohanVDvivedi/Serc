# project name
PROJECT_NAME:=serc

# this is the place where we download in your system
DOWNLOAD_DIR:=/usr/local

# we may download all the public headers

# list of public api headers (only these headers will be installed)
PUBLIC_HEADERS:=http_server.h http_client_set.h distributer.h server_global_params.h\
				strhsh.h file_request_resolution_controller.h
# these are the tools that will be used for helping you create your own distributer file
ROUTING_SCRIPT:=route.py
ROUTING_TEMPLATE:=distributer_source.temp
# the library, which we will create
LIBRARY:=lib${PROJECT_NAME}.a
# the binary, which will use the created library
BINARY:=${PROJECT_NAME}

# list of all the directories, in the project
INC_DIR:=./inc
OBJ_DIR:=./obj
LIB_DIR:=./lib
SRC_DIR:=./src
BIN_DIR:=./bin
CON_DIR:=./con
PYT_DIR:=./pyt
TMP_DIR:=./share

# compiler
CC:=gcc
# compiler flags
CFLAGS:=-Wall -O3 -I${INC_DIR}
# linker flags, this will used to compile the binary
LFLAGS:=-L${LIB_DIR} -l${PROJECT_NAME} -lhttpparser -lconnman -lz -lssl -lcrypto -lboompar -lpthread -lcutlery
# Archiver
AR:=ar rcs

# utility
RM:=rm -f
MK:=mkdir -p
CP:=cp

# sources and objects must be evaluated every time you use them
# figure out all the sources in the project
SOURCES=$(shell find ${SRC_DIR} -name '*.c')
# and the required objects to be built, as intermediary
OBJECTS=$(patsubst ${SRC_DIR}/%.c, ${OBJ_DIR}/%.o, ${SOURCES})

# rule to make the directory for storing object files, that we create
${OBJ_DIR} :
	${MK} $@

# generic rule to build any object file
${OBJ_DIR}/%.o : ${SRC_DIR}/%.c | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@

# rule to make the directory for storing libraries, that we create
${LIB_DIR} :
	${MK} $@

# generic rule to make a library
${LIB_DIR}/${LIBRARY} : ${OBJECTS} | ${LIB_DIR}
	${AR} $@ ${OBJECTS}

# rule to make the directory for storing binaries, that we create
${BIN_DIR} :
	${MK} $@

# the routing is constructed by this dependency make statement
./distributer.c : ${CON_DIR}/*.con ${PYT_DIR}/${ROUTING_SCRIPT} ${TMP_DIR}/${ROUTING_TEMPLATE}
	${PYT_DIR}/${ROUTING_SCRIPT} $^

# generic rule to make a binary using the library that we just created
${BIN_DIR}/${BINARY} : ./server.c ./distributer.c ${LIB_DIR}/${LIBRARY} | ${BIN_DIR}
	${CC} ${CFLAGS} ./server.c ./distributer.c ${LFLAGS} -o $@

# building a a test client
./test_client : ./test_client.c
	${CC} ${CFLAGS} $< ${LFLAGS} -ljsonparser -o $@

# to build the binary along with the library
all : ${BIN_DIR}/${BINARY}

# clean all the build, in this directory
clean :
	${RM} -r ${BIN_DIR} ${LIB_DIR} ${OBJ_DIR}

# -----------------------------------------------------
# INSTALLING and UNINSTALLING system wide
# -----------------------------------------------------

PUBLIC_HEADERS_TO_INSTALL=$(patsubst %.h, ${INC_DIR}/${PROJECT_NAME}/%.h, ${PUBLIC_HEADERS})

# install the library, from this directory to user environment path
# you must uninstall current installation before making a new installation
install : uninstall all
	# include
	${MK} ${DOWNLOAD_DIR}/include/${PROJECT_NAME}
	${CP} ${PUBLIC_HEADERS_TO_INSTALL} 						${DOWNLOAD_DIR}/include/${PROJECT_NAME}
	# lib
	${MK} ${DOWNLOAD_DIR}/lib
	${CP} ${LIB_DIR}/${LIBRARY} 							${DOWNLOAD_DIR}/lib
	# bin
	${MK} ${DOWNLOAD_DIR}/bin
	${CP} ${BIN_DIR}/${BINARY} ${PYT_DIR}/${ROUTING_SCRIPT}	${DOWNLOAD_DIR}/bin
	# share
	${MK} ${DOWNLOAD_DIR}/share
	${CP} ${TMP_DIR}/${ROUTING_TEMPLATE} 					${DOWNLOAD_DIR}/share

# removes what was installed
uninstall : 
	${RM} -r ${DOWNLOAD_DIR}/include/${PROJECT_NAME}
	${RM} ${DOWNLOAD_DIR}/lib/${LIBRARY}
	${RM} ${DOWNLOAD_DIR}/bin/${BINARY} ${DOWNLOAD_DIR}/bin/${ROUTING_SCRIPT}
	${RM} ${DOWNLOAD_DIR}/share/${ROUTING_TEMPLATE}

# -----------------------------------------------------
# Creating a self signed certificate
# -----------------------------------------------------

# builds a self signed ssl key and certificate for your server
ssl_cert :
	openssl req -new -x509 -pubkey -newkey rsa:4096 -days 365 -nodes -keyout ${CON_DIR}/server.key -out ${CON_DIR}/server.crt