# C alsa wrapper
It's a header only lib.

# Example
```c
#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

int main() {
	audio_device_t *device = audio_device_create("pulse");
	int ret = audio_device_open(device, INPUT);  // Or OUTPUT
	if (ret == 0) {
		goto fail;
	}

	audio_device_set_begin(device);
	audio_device_set_rate(device, 8000);
	audio_device_set_channel(device, 1);
	audio_device_set_bit_depth(device, 8);
	audio_device_set_end(device);

	while (1) {
		size_t size;
		[[maybe_unused]]
		char *buffer = audio_device_read(device, &size);

        // TODO use your buffer
	}

fail:
	audio_device_destroy(device);
    return 0;
}
```
