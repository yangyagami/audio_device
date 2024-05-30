#ifndef AUDIO_DEVICE_AUDIO_DEVICE_H_
#define AUDIO_DEVICE_AUDIO_DEVICE_H_

#ifdef __cpluscplus
extern "C" {
#endif

enum audio_device_type {
	INPUT = 0,
	OUTPUT,
};
typedef enum audio_device_type audio_device_type_t;

struct audio_device;
typedef struct audio_device audio_device_t;

audio_device_t *audio_device_create(const char *device_name);
void audio_device_destroy(audio_device_t *device);

int audio_device_open(audio_device_t *device, audio_device_type_t type);
void audio_device_close(audio_device_t *device);

void audio_device_set_begin(audio_device_t *device);
void audio_device_set_rate(audio_device_t *device, int rate);
void audio_device_set_bit_depth(audio_device_t *device, int bit_depth);
void audio_device_set_channel(audio_device_t *device, int channel);
void audio_device_set_period_size(audio_device_t *device, int period_size);
void audio_device_set_period_time(audio_device_t *device, int period_time_ms);
void audio_device_set_buffer_size(audio_device_t *device, int size);
void audio_device_set_buffer_time(audio_device_t *device, int time_ms);
void audio_device_set_end(audio_device_t *device);

int audio_device_get_rate(audio_device_t *device);
int audio_device_get_channel(audio_device_t *device);
int audio_device_get_bit_depth(audio_device_t *device);

unsigned int audio_device_read(audio_device_t *device, void *buffer, unsigned int frame);
void audio_device_write(audio_device_t *device, void *buffer, unsigned int frame);

#ifdef __cpluscplus
}
#endif

#endif  // AUDIO_DEVICE_AUDIO_DEVICE_H_

#ifdef AUDIO_DEVICE_IMPLEMENTATION

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <alsa/asoundlib.h>

struct audio_device {
	audio_device_type_t type;
	snd_pcm_t *device_handle;
	snd_pcm_hw_params_t *hw_params;
	char device_name[256];
	int rate;
	int channel;
	int bit_depth;
	int period_size;
	int period_time_ms;
	int buffer_size;
	int buffer_time_ms;
};

audio_device_t *audio_device_create(const char *device_name) {
	audio_device_t *device = NULL;
	device = (audio_device_t *) calloc(1, sizeof(*device));

	strcpy(device->device_name, device_name);

	return device;
}

void audio_device_destroy(audio_device_t *device) {
	assert(device != NULL);

	audio_device_close(device);

	free(device);
}

int audio_device_open(audio_device_t *device, audio_device_type_t type) {
	assert(device != NULL);

	snd_pcm_stream_t stream_type;
	if (type == INPUT) {
		stream_type = SND_PCM_STREAM_CAPTURE;
	} else {
		stream_type = SND_PCM_STREAM_PLAYBACK;
	}
	device->type = type;

	// block open
	int ret = snd_pcm_open(
		&device->device_handle,
		device->device_name,
		stream_type,
		0);
	if (ret < 0) {
		fprintf(stderr, "Cannot open device: %s, err: %s\n",
			device->device_name, snd_strerror(ret));
		return 0;
	}

	return 1;
}

void audio_device_close(audio_device_t *device) {
	if (device->device_handle != NULL) {
		if (device->type == OUTPUT) {
			int err = snd_pcm_drain(device->device_handle);
			if (err < 0) {
				printf("snd_pcm_drain failed: %s\n",
				       snd_strerror(err));
			}
		}
		snd_pcm_close(device->device_handle);
	}
}

void audio_device_set_begin(audio_device_t *device) {
	assert(device != NULL);

	// 分配硬件参数结构体
	snd_pcm_hw_params_malloc(&device->hw_params);

	// 初始化硬件参数结构体
	snd_pcm_hw_params_any(device->device_handle, device->hw_params);

	// 设置硬件参数
	snd_pcm_hw_params_set_access(
		device->device_handle,
		device->hw_params,
		SND_PCM_ACCESS_RW_INTERLEAVED);
}

void audio_device_set_rate(audio_device_t *device, int rate) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->rate = rate;

	snd_pcm_hw_params_set_rate(
		device->device_handle,
		device->hw_params,
		device->rate,
		0);
}

void audio_device_set_bit_depth(audio_device_t *device, int bit_depth) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->bit_depth = bit_depth;

	snd_pcm_format_t format;
	switch (device->bit_depth) {
		case 8: {
			format = SND_PCM_FORMAT_U8;
			break;
		}
		case 16: {
			format = SND_PCM_FORMAT_S16_LE;
			break;
		}
		case 32: {
			format = SND_PCM_FORMAT_S32_LE;
			break;
		}
	}

	snd_pcm_hw_params_set_format(
		device->device_handle,
		device->hw_params,
		format);
}

void audio_device_set_channel(audio_device_t *device, int channel) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->channel = channel;

	snd_pcm_hw_params_set_channels(
		device->device_handle,
		device->hw_params,
		device->channel);
}

void audio_device_set_period_size(audio_device_t *device, int period_size) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->period_size = period_size;

	snd_pcm_hw_params_set_period_size(
		device->device_handle,
		device->hw_params,
		device->period_size,
		0);
}

void audio_device_set_period_time(audio_device_t *device, int period_time_ms) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->period_time_ms = period_time;

	snd_pcm_hw_params_set_period_time(
		device->device_handle,
		device->hw_params,
		device->period_time_ms * 1000,
		0);
}

void audio_device_set_buffer_size(audio_device_t *device, int size) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->buffer_size = size;

	snd_pcm_hw_params_set_buffer_size(
		device->device_handle,
		device->hw_params,
		device->buffer_size);
}

void audio_device_set_buffer_time(audio_device_t *device, int time_ms) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->buffer_time_ms = time_ms;

	snd_pcm_hw_params_set_buffer_time(
		device->device_handle,
		device->hw_params,
		device->buffer_time_ms * 1000,
		0);
}

void audio_device_set_end(audio_device_t *device) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	snd_pcm_hw_params(device->device_handle, device->hw_params);
	snd_pcm_hw_params_free(device->hw_params);
	snd_pcm_prepare(device->device_handle);
}

int audio_device_get_rate(audio_device_t *device) {
	assert(device != NULL);

	return device->rate;
}

int audio_device_get_channel(audio_device_t *device) {
	assert(device != NULL);

	return device->channel;
}

int audio_device_get_bit_depth(audio_device_t *device) {
	assert(device != NULL);

	return device->bit_depth;
}

unsigned int audio_device_read(audio_device_t *device, void *buffer, unsigned int frame) {
	assert(device != NULL);

	int ret = snd_pcm_readi(
		device->device_handle,
		buffer,
	        frame);
	if (ret == -EPIPE) {
		fprintf(stderr, "overrun occurred\n");
		snd_pcm_prepare(device->device_handle);
		goto fail;
	} else if (ret < 0) {
		fprintf(stderr, "Cannot read data from device: %s\n",
			snd_strerror(ret));
		goto fail;
	} else if (ret != frame) {
		fprintf(stderr, "short read, read %d frames\n", ret);
	}

	return ret;
fail:
	return 0;
}

void audio_device_write(audio_device_t *device, void *buffer, unsigned int frame) {
	assert(device != NULL);

	int ret = snd_pcm_writei(
		device->device_handle,
		buffer,
		frame);
	if (ret < 0) {
		fprintf(stderr, "Ret less then 0, do recover\n");
		ret = snd_pcm_recover(device->device_handle, ret, 0);
	}
	if (ret < 0) {
		fprintf(stderr, "Write data to device failed: %s\n",
			snd_strerror(ret));
	}
}

#undef AUDIO_DEVICE_AUDIO_DEVICE_FRAMES

#endif  // AUDIO_DEVICE_IMPLEMENTATION
