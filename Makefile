test_bin: test/test.c audio_device.h
	gcc -o $@ test/test.c -Wall -Werror -I. -fsanitize=address -lasound
