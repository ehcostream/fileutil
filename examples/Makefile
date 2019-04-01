include ../src/

APP_bin=worldsvr.so
TARGET_DIR=${PROJ_HOME}/Release/WorldSvr/lib

APP_SRC=$(wildcard *.cpp)
APP_INC=-I$(TSF4G_INC_DIR) -I$(TSF4WG_INC_DIR) -I$(PROJ_INC_DIR) -I./ 


APP_LIB= -L$(TSF4G_LIB_DIR) -L$(PROJ_LIB_DIR) -L.  \
         -lgameconfig -lgamenet -lcommon -lwebsocket -lgameproto -ldb -lresloader \
         -L/usr/local/lib -Wl,-Bstatic -lprotobuf $(TSF4G_LIBS) \
         -Wl,-Bdynamic -liconv -lz -lc -lpthread

	
APP_OBJ=$(APP_SRC:.cpp=.o)

ifeq ($(ver), release)
	CPPFLAGS+=-D RELEASE
endif

.PHONY: all clean install

all: $(APP_bin)

-include $(APP_SRC:.cpp=.d)

%.d: %.cpp
	$(CC) -MM $(CPPFLAGS) $(APP_INC) $< -o $@

%.o: %.cpp %.d
	$(CC) -c $(CPPFLAGS) $(APP_INC) $< -o $@

$(APP_bin): $(APP_OBJ)
	$(CC) -o $@ $(CPPFLAGS) $^ $(APP_LIB)

install:
	@if [ ! -d ${TARGET_DIR} ]; then mkdir -p ${TARGET_DIR}; fi
	cp -f $(APP_bin) ${TARGET_DIR}/


clean:
	-$(RM)  *.o  
	-$(RM)  *.d 
	-$(RM)  *.a
	-$(RM)  $(APP_bin)

