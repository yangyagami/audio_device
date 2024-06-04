#include <stdio.h>
#include <unistd.h>

#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

int main() {
	audio_device_t *record_device = audio_device_create("pulse");
	audio_device_t *play_device = audio_device_create("pulse");

	int ret = audio_device_open(record_device, INPUT);
	if (ret == 0) {
		goto fail;
	}
	ret = audio_device_open(play_device, OUTPUT);
	if (ret == 0) {
		goto fail;
	}
	audio_device_set_begin(record_device);
	audio_device_set_rate(record_device, 16000);
	audio_device_set_channel(record_device, 1);
	audio_device_set_bit_depth(record_device, 16);
	audio_device_set_end(record_device);

	audio_device_set_begin(play_device);
	audio_device_set_rate(play_device, 16000);
	audio_device_set_channel(play_device, 1);
	audio_device_set_bit_depth(play_device, 16);
	audio_device_set_buffer_time(play_device, 20);
	audio_device_set_period_time(play_device, 10);
	audio_device_set_end(play_device);

	while (1) {
		int16_t buffer[160] = { 0 };
		unsigned int frame = audio_device_read(record_device, buffer, sizeof(buffer) / sizeof(buffer[0]));

		audio_device_write(play_device, buffer, frame);
	}


fail:
	audio_device_destroy(record_device);
	audio_device_destroy(play_device);
	return 0;
}
