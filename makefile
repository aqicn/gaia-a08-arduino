
SERIAL = /dev/cu.usbmodem101
FQBN = esp32:esp32:esp32c3:CDCOnBoot=cdc

all: compile-arduino


compile-arduino:
	arduino-cli compile --fqbn $(FQBN)
	arduino-cli upload -p $(SERIAL) --fqbn $(FQBN)
	python -m serial.tools.miniterm $(SERIAL) 115200

compile-platformio:
	pio run -t upload --upload-port $(SERIAL)
	pio device monitor

monitor:
	python -m serial.tools.miniterm $(SERIAL) 115200
