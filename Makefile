test_cpp: test/test_cpp.cc audio_device.h
	g++ -o $@ test/test_cpp.cc -Wall -Werror -I. -fsanitize=address -lasound
test_record_play: test/test_record_play.c audio_device.h
	gcc -o $@ test/test_record_play.c -Wall -Werror -I. -fsanitize=address -lasound
