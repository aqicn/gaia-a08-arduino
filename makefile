
SERIAL = /dev/cu.usbmodem101
FQBN = esp32:esp32:esp32c3:CDCOnBoot=cdc

all:
	arduino-cli compile --fqbn $(FQBN)
	arduino-cli upload -p $(SERIAL) --fqbn $(FQBN)
	python -m serial.tools.miniterm $(SERIAL) 115200


monitor:
	python -m serial.tools.miniterm $(SERIAL) 115200
