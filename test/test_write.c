#include <stdio.h>
#include <unistd.h>

#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

int main() {
	audio_device_t *device = audio_device_create("pulse");

	int ret = audio_device_open(device, INPUT);
	if (ret == 0) {
		goto fail;
	}
	audio_device_set_begin(device);
	audio_device_set_rate(device, 16000);
	audio_device_set_channel(device, 1);
	audio_device_set_bit_depth(device, 16);
	audio_device_set_end(device);

	int count = 0;
	FILE *file = fopen("test.pcm", "wb");
	while (1) {
		int16_t buffer[1024] = { 0 };
		unsigned int frame = audio_device_read(device, buffer, 128);

		fwrite(buffer, 1, frame * sizeof(buffer[0]), file);
		printf("cnt: %d\n", count);
		if (count++ >= 320) {
			break;
		}
	}
	fclose(file);


fail:
	audio_device_destroy(device);
	return 0;
}
