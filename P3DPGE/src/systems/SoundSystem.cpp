#include "SoundSystem.h"
#include "ConsoleSystem.h"

#include "../components/Camera.h"
#include "../components/Source.h"

#include <thread>

#define DR_WAV_IMPLEMENTATION
#include "../internal/draudio/dr_wav.h"

#define TEST_ERROR check_al_errors(__FILE__, __LINE__, admin)

//list all data devices found on the system
static void list_audio_devices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}

//checking OpenAL Errors. used with TEST_ERROR
bool check_al_errors(const std::string& filename, const std::uint_fast32_t line, EntityAdmin* admin)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		CERROR("***ERROR*** (", filename, ": ", line, ")\n");
		switch (error)
		{
		case AL_INVALID_NAME:
			CERROR("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
			break;
		case AL_INVALID_ENUM:
			CERROR("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
			break;
		case AL_INVALID_VALUE:
			CERROR("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
			break;
		case AL_INVALID_OPERATION:
			CERROR("AL_INVALID_OPERATION: the requested operation is not valid");
			break;
		case AL_OUT_OF_MEMORY:
			CERROR("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
			break;
		default:
			CERROR("UNKNOWN AL ERROR: ", error);
		}
		return false;
	}
	return true;
}

void play_sound(Source* s) {
	
	


	float x, y, z;

	alSourcePlay(s->source);
	//get source state
	alGetSourcei(s->source, AL_SOURCE_STATE, &s->source_state);
	alGetSource3f(s->source, AL_POSITION, &x, &y, &z);
	


	while (s->source_state == AL_PLAYING) {
		Vector3 pos;
		Vector3 vel = Vector3::ZERO;

		LOG(x, " ", y, " ", z);

		if (s->physpoint) {
			pos = s->p->position;
			vel = s->p->velocity;
		}
		else {
			pos = s->t->position;
		}

		alSource3f(s->source, AL_POSITION, pos.x, pos.y, pos.z);
		alSource3f(s->source, AL_VELOCITY, vel.x, vel.y, vel.z);

		alGetSourcei(s->source, AL_SOURCE_STATE, &s->source_state);
	}
}


void CollectSources(std::vector<ALuint*>& buffers, EntityAdmin* admin) {
	for (auto e : admin->entities) {
		for (auto c : e.second->components) {
			if (Source* s = dynamic_cast<Source*>(c)) {
				if (!alIsSource(s->source)) {
					Vector3 pos;
					Vector3 vel = Vector3::ZERO;
					if (s->physpoint) {
						pos = s->p->position;
						vel = s->p->velocity;
					}
					else {
						pos = s->t->position;
					}

					//generate source and set its data
					alGenSources((ALuint)1, &s->source);
					TEST_ERROR;
					alSourcef(s->source, AL_PITCH, s->pitch);
					TEST_ERROR;
					alSourcef(s->source, AL_GAIN, s->gain);
					TEST_ERROR;
					alSource3f(s->source, AL_POSITION, pos.x, pos.y, pos.z);
					TEST_ERROR;
					alSource3f(s->source, AL_VELOCITY, vel.x, vel.y, vel.z);
					TEST_ERROR;
					alSourcei(s->source, AL_LOOPING, s->loop);
					TEST_ERROR;

					//generate buffer
					ALuint* buffer = new ALuint;
					alGenBuffers(1, buffer);
					TEST_ERROR;

					//initialize file and get it's data
					drwav file;
					drwav_init_file(&file, s->snd_file, NULL);

					ALenum format;
					unsigned int channels = file.channels;
					unsigned int sampleRate = file.sampleRate;
					drwav_uint64 totalPCMFrameCount = file.totalPCMFrameCount;
					std::uint8_t bitsPerSample = file.bitsPerSample;

					drwav_int16* decoded = (drwav_int16*)malloc(file.totalPCMFrameCount * file.channels * sizeof(drwav_int16));
					size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s16(&file, file.totalPCMFrameCount, decoded);

					//ALenum format;
					if (channels == 1 && bitsPerSample == 8)
						format = AL_FORMAT_MONO8;
					else if (channels == 1 && bitsPerSample == 16)
						format = AL_FORMAT_MONO16;
					else if (channels == 2 && bitsPerSample == 8)
						format = AL_FORMAT_STEREO8;
					else if (channels == 2 && bitsPerSample == 16)
						format = AL_FORMAT_STEREO16;
					else {
						CASSERT(false, "unrecognized audio file format");
					}

					//put data in buffer
					alBufferData(*buffer, format, decoded, totalPCMFrameCount, sampleRate);
					TEST_ERROR;

					//bind buffer to source
					alSourcei(s->source, AL_BUFFER, *buffer);
					TEST_ERROR;

					free(decoded);

					
					//buffers.push_back(buffer);


				}
			}
		}
	}
}

void SoundSystem::Init() {
	ALboolean enumeration;
	
	//ALsizei size, freq;
	//ALenum format;
	//ALuint buffer, source;
	//ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	//ALboolean loop = AL_FALSE;
	ALCenum error;
	ALint source_state;

	//fprintf(stdout, "Using " BACKEND " as audio backend\n");

	//can we enumerate audio devices?
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
		CLOG_LOC("OpenAL unable to enumerate devices");

	//list audio devices
	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	if (!defaultDeviceName)
		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	//attempt to choose a device
	device = alcOpenDevice(defaultDeviceName);
	TEST_ERROR;
	CASSERT(device, "unable to open a device with OpenAL");
	
	//display audio device selected in console
	CLOG("Audio device selected: ", alcGetString(device, ALC_DEVICE_SPECIFIER));

	//attempt to create OpenAL context
	context = alcCreateContext(device, NULL);
	TEST_ERROR;
	CASSERT(alcMakeContextCurrent(context), "unable to make OpenAL context");

	//collect all sources at startup
	//TODO(, sushi) if its necessary, make a way to collect new sources and to remove them
	CollectSources(buffers, admin);
	
}

void SoundSystem::Update() {

	Camera* c = admin->currentCamera;
	
	//collect any new sources
	CollectSources(buffers, admin);
	Vector3 ld = c->lookDir;
	Vector3 up = c->up;
	LOG(ld, " ", up);
	ALfloat listenerOri[] = { ld.x, ld.z, ld.y, up.x, up.z, up.y };
	//set OpenAL's listener to match our camera
	//right now it's only position cause that's all we have with the camera
	alListener3f(AL_POSITION, c->position.x, c->position.y, c->position.z);
	TEST_ERROR;
	//alListener3f(AL_VELOCITY, 0, 0, 0);
	//TEST_ERROR;
	alListenerfv(AL_ORIENTATION, listenerOri);
	TEST_ERROR;

	for (auto e : admin->entities) {
		for (auto c : e.second->components) {
			if (Source* s = dynamic_cast<Source*>(c)) {
				if (s->source_state != AL_PLAYING) {
					//TODO(, sushi) learn more about threads and closing them properly
					std::thread thread_obj(play_sound, s);
					thread_obj.detach();
				}
			}
		}
	}
	


}