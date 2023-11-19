
SERIAL = /dev/cu.usbmodem101

all:
	arduino-cli compile --fqbn esp32:esp32:esp32c3 
	arduino-cli upload -p $(SERIAL) --fqbn esp32:esp32:esp32c3 
	python -m serial.tools.miniterm $(SERIAL) 115200


monitor:
	python -m serial.tools.miniterm $(SERIAL) 115200
