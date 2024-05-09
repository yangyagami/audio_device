#include <stdio.h>
#include <unistd.h>

#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

int main() {
	audio_device_t *device = audio_device_create("pulse");
	audio_device_t *output_device = audio_device_create("pulse");


	int ret = audio_device_open(device, INPUT);
	if (ret == 0) {
		goto fail;
	}
	ret = audio_device_open(output_device, OUTPUT);
	if (ret == 0) {
		goto fail;
	}

	audio_device_set_begin(device);
	audio_device_set_rate(device, 16000);
	audio_device_set_channel(device, 1);
	audio_device_set_bit_depth(device, 16);
	audio_device_set_end(device);

	audio_device_set_begin(output_device);
	audio_device_set_rate(output_device, 16000);
	audio_device_set_channel(output_device, 1);
	audio_device_set_bit_depth(output_device, 16);
	audio_device_set_buffer_time(output_device, 512);
	audio_device_set_period_time(output_device, 40);
	audio_device_set_end(output_device);

	while (1) {
		int16_t buffer[1024] = { 0 };
		unsigned int frame = audio_device_read(device, buffer, 16);
		for (size_t i = 0; i < ARRAY_LEN(buffer); i++) {
			printf("0x%02x ", buffer[i]);
		}
		printf("\n");
		if (frame > 0) {
			audio_device_write(output_device, buffer, frame);
		}
	}


fail:
	audio_device_destroy(device);
	audio_device_destroy(output_device);
	return 0;
}

#undef ARRAY_LEN
