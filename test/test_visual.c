#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <raylib.h>

#define AUDIO_DEVICE_IMPLEMENTATION
#include "audio_device.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

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

	const int ww = 900;
	const int wh = 600;
	[[maybe_unused]]
	const int cwx = ww / 2;
	const int cwy = wh / 2;
	InitWindow(ww, wh, "Audio Visual");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		int16_t buffer[300] = { 0 };
		unsigned int frame = audio_device_read(device, buffer, ARRAY_LEN(buffer));

		BeginDrawing();
		ClearBackground(WHITE);

		int cell_width = ww / frame;
		for (size_t i = 0; i < frame; i++) {
			int cell_height = (float) abs(buffer[i]) / 0x7FFF * (wh / 2);
			if (cell_height < 3) cell_height = 3;
			DrawRectangle((cell_width) * i, cwy - cell_height / 2, cell_width, cell_height, BLUE);
		}

		EndDrawing();
	}

fail:
	audio_device_destroy(device);
	CloseWindow();
	return 0;
}
