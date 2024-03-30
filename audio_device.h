#ifndef AUDIO_DEVICE_AUDIO_DEVICE_H_
#define AUDIO_DEVICE_AUDIO_DEVICE_H_

#ifdef __cpluscplus
extern "C" {
#endif

enum audio_device_type;
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

char *audio_device_read(audio_device_t *device, size_t *size);
void audio_device_write(audio_device_t *device, char *buffer, size_t size);

#ifdef __cpluscplus
}
#endif

#endif  // AUDIO_DEVICE_AUDIO_DEVICE_H_

#ifdef AUDIO_DEVICE_IMPLEMENTATION

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <alsa/asoundlib.h>

#define AUDIO_DEVICE_AUDIO_DEVICE_FRAMES (128)

enum audio_device_type {
	INPUT = 0,
	OUTPUT,
};

struct audio_device {
	snd_pcm_t *device_handle;
	snd_pcm_hw_params_t *hw_params;
	char *buffer;
	const char *device_name;
	int rate;
	int channel;
	int bit_depth;
	int period_size;
	int period_time;
	int buffer_size;
	int buffer_time;
};

audio_device_t *audio_device_create(const char *device_name) {
	audio_device_t *device = NULL;
	device = calloc(1, sizeof(*device));

	device->device_name = device_name;
	device->buffer = calloc(1, 1024);

	return device;
}

void audio_device_destroy(audio_device_t *device) {
	assert(device != NULL);

	audio_device_close(device);

	free(device->buffer);
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
		int err = snd_pcm_drain(device->device_handle);
		if (err < 0) {
			printf("snd_pcm_drain failed: %s\n",
			       snd_strerror(err));
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
			format = SND_PCM_FORMAT_U16_LE;
			break;
		}	
		case 32: {
			format = SND_PCM_FORMAT_U32_LE;
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

void audio_device_set_period_time(audio_device_t *device, int period_time) {
	assert(device != NULL);

	assert(device->hw_params != NULL);

	device->period_time = period_time;

	snd_pcm_hw_params_set_period_time(
		device->device_handle,
		device->hw_params,
		device->period_time,
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

	device->buffer_time = time_ms;

	snd_pcm_hw_params_set_buffer_size(
		device->device_handle,
		device->hw_params,
		device->buffer_time);
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

char *audio_device_read(audio_device_t *device, size_t *size) {
	assert(device != NULL);

	int ret = snd_pcm_readi(
		device->device_handle,
		device->buffer,
		AUDIO_DEVICE_AUDIO_DEVICE_FRAMES);
	if (ret == -EPIPE) {
		fprintf(stderr, "overrun occurred\n");
		snd_pcm_prepare(device->device_handle);
	} else if (ret < 0) {
		fprintf(stderr, "Cannot read data from device: %s\n",
			snd_strerror(ret));
	} else if (ret != AUDIO_DEVICE_AUDIO_DEVICE_FRAMES) {
		fprintf(stderr, "short read, read %d frames\n", ret);
		*size = ret;
		return device->buffer;
	} else {
		*size = ret;
		return device->buffer;
	}

	*size = 0;
	return NULL;
}

void audio_device_write(audio_device_t *device, char *buffer, size_t size) {
	assert(device != NULL);

	int ret = snd_pcm_writei(
		device->device_handle,
		buffer,
		size);
	if (ret < 0) {
		ret = snd_pcm_recover(device->device_handle, ret, 0);
	}
	if (ret < 0) {
		fprintf(stderr, "Write data to device failed: %s\n",
			snd_strerror(ret));
	}
}

#undef AUDIO_DEVICE_AUDIO_DEVICE_FRAMES

#endif  // AUDIO_DEVICE_IMPLEMENTATION
