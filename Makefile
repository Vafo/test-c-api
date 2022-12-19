BUILDDIR ?= build
DEPDIR ?= dep

TARGET_SERVER := server
SERVER_SRC := server.c
# EXCLUDE HEADER FILES WITH NO C IMPLEMENTATION
SERVER_HEADER_SRC := $(filter %.h,$(shell gcc -MM $(SERVER_SRC)))
SERVER_HEADER_SRC := $(SERVER_HEADER_SRC:.h=.c)
SERVER_CUSTOM_SRC := 
SERVER_SRCS := $(SERVER_SRC) $(SERVER_HEADER_SRC) $(SERVER_CUSTOM_SRC)

TARGET_CLIENT := client
CLIENT_SRC := client.c
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

abobus:
	@echo "SERVER SRCS :" $(SERVER_SRCS)
	@echo "SERVER OBJS :" $(SERVER_OBJS)
	@echo "CLIENT SRCS :" $(CLIENT_SRCS)
	@echo "CLIENT OBJS :" $(CLIENT_OBJS)


$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -c $< -o $@

$(DEPDIR)/%.d: %.c
	@mkdir -p $(dir $@)
	gcc -MM $^ -MT '$(BUILDDIR)/'$(^:.c=.o) -o $@

$(TARGET_SERVER): $(SERVER_OBJS)
	gcc -o $@ $^

$(TARGET_CLIENT): $(CLIENT_OBJS)
	gcc -o $@ $^

build_server: $(TARGET_SERVER)
	@echo "SERVER ABOBUS"

build_client: $(TARGET_CLIENT)
	@echo "CLIENT ABOBUS"

clean:
	rm -r -f -v $(TARGET_CLIENT) $(TARGET_SERVER) $(DEPDIR) $(BUILDDIR)

.PHONY: build_server build_client clean