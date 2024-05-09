test_bin: test/test.c audio_device.h
	gcc -o $@ test/test.c -Wall -Werror -I. -fsanitize=address -lasound
test_write: test/test_write.c audio_device.h
	gcc -o $@ test/test_write.c -Wall -Werror -I. -fsanitize=address -lasound
test_read: test/test_read.c audio_device.h
	gcc -o $@ test/test_read.c -Wall -Werror -I. -fsanitize=address -lasound
