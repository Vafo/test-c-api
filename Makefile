BUILDDIR ?= build
DEPDIR ?= dep
SERVER_DIR = ./server
CLIENT_DIR = ./client

TARGET_SERVER := server_app
SERVER_SRC := $(shell find $(SERVER_DIR) -name "*.c")
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
SERVER_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(SERVER_SRC)))
SERVER_HEADER_SRC := $(SERVER_HEADER_SRC:.h=.c)
# SERVER_CUSTOM_SRC := $(shell find ./http -name "*.c")
SERVER_SRCS := $(SERVER_SRC) $(SERVER_HEADER_SRC) $(SERVER_CUSTOM_SRC)

TARGET_CLIENT := client_app
CLIENT_SRC := $(shell find $(CLIENT_DIR) -name "*.c")
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
CLIENT_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(CLIENT_SRC)))
CLIENT_HEADER_SRC := $(CLIENT_HEADER_SRC:.h=.c)
CLIENT_CUSTOM_SRC :=
CLIENT_SRCS := $(CLIENT_SRC) $(CLIENT_HEADER_SRC) $(CLIENT_CUSTOM_SRC) 


DEPS := $(($(SERVER_SRCS) $(CLIENT_SRCS)):.c=.d)
DEPS := $(addprefix $(DEPDIR)/, $(DEPS))


SERVER_OBJS := $(SERVER_SRCS:.c=.o)
SERVER_OBJS := $(addprefix $(BUILDDIR)/, $(SERVER_OBJS))
CLIENT_OBJS := $(CLIENT_SRCS:.c=.o)
CLIENT_OBJS := $(addprefix $(BUILDDIR)/, $(CLIENT_OBJS))

JSON_C_DIR=/usr/local
# CFLAGS += -I$(JSON_C_DIR)/include/json-c
# Or to use lines like: #include <json-c/json_object.h>
CLIENT_CFLAGS += -I$(JSON_C_DIR)/include
CLIENT_LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -c $< -o $@

$(DEPDIR)/%.d: %.c
	@mkdir -p $(dir $@)
	gcc -MM $^ -MT '$(BUILDDIR)/'$(^:.c=.o) -o $@

$(TARGET_SERVER): $(SERVER_OBJS)
	gcc -o $@ $^

$(TARGET_CLIENT): $(CLIENT_OBJS)
	gcc -o $@ $(CLIENT_CFLAGS)  $^ $(CLIENT_LDFLAGS)

# build-server: $(TARGET_SERVER)
# 	@echo "SERVER ABOBUS"

build-server: $(TARGET_SERVER)
	@echo "Build : server"

build-client: $(TARGET_CLIENT)
	@echo "Build : client"

build: build-client build-server

clean:
	rm -r -f -v $(TARGET_CLIENT) $(TARGET_SERVER) $(DEPDIR) $(BUILDDIR)

.PHONY: build-server build-client build clean