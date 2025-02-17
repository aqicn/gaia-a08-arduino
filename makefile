
SERIAL = /dev/cu.usbmodem101

all: compile-platformio

compile-arduino:
	@echo "Arduino compilation is not supported on this branch. Check the readme for more information"

compile-platformio:
	pio run -t upload --upload-port $(SERIAL)
	pio device monitor

monitor:
	python -m serial.tools.miniterm $(SERIAL) 115200
