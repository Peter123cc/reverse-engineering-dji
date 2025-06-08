CFLAGS?=$(shell pkg-config --cflags-only-I wireshark)

build: dji-ble-message.so

install: build
	mkdir -p ~/.local/lib/wireshark/plugins/4.2/epan
	cp dji-ble-message.so ~/.local/lib/wireshark/plugins/4.2/epan


%.so : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -shared $< -o $@

fmt:
	astyle *.c
	rm -f *.c.orig
