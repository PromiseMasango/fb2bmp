all:
	arm-linux-androideabi-gcc fb2bmp.c -o fb2bmp
send:
	adb push fb2bmp /data/local/tmp/
