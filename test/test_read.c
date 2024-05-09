#include <stdio.h>
#include <unistd.h>

#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

int main() {
	audio_device_t *device = audio_device_create("pulse");

	int ret = audio_device_open(device, OUTPUT);
	if (ret == 0) {
		goto fail;
	}
	audio_device_set_begin(device);
	audio_device_set_rate(device, 16000);
	audio_device_set_channel(device, 1);
	audio_device_set_bit_depth(device, 16);
	audio_device_set_buffer_time(device, 1000);
	audio_device_set_period_time(device, 200);
	audio_device_set_end(device);

	FILE *file = fopen("test.pcm", "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	int16_t buffer[1024 * 1024];
	fread(buffer, 1, size, file);

	audio_device_write(device, buffer, size / sizeof(int16_t));

	fclose(file);


fail:
	audio_device_destroy(device);
	return 0;
}
