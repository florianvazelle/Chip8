#include "Audio.hpp"

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <array>
#include <stdexcept>

Audio::Audio() : m_source(0), m_buffer(0), m_device(nullptr), m_context(nullptr) {
    // Opening the device
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        throw std::runtime_error("We cannot open an audio device!");
    }

    // Creating the context
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        throw std::runtime_error("We cannot create an audio context!");
    }

    // Context activation
    if (!alcMakeContextCurrent(m_context)) {
        throw std::runtime_error("We cannot activate the audio context!");
    }

    // Creating a source
    alGenSources(1, &m_source);

    // Creation of the OpenAL buffer
    alGenBuffers(1, &m_buffer);
    createBuffer();

    alSourcef(m_source, AL_PITCH, 1);
    alSourcef(m_source, AL_GAIN, 1);
    alSource3f(m_source, AL_POSITION, 0, 0, 0);
    alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
    alSourcei(m_source, AL_LOOPING, AL_FALSE);

    // We attach the buffer containing the audio samples to the source
    alSourcei(m_source, AL_BUFFER, m_buffer);
}

Audio::~Audio() {
    // Destruction of the buffer
    alDeleteBuffers(1, &m_buffer);

    // Destruction of the source
    alSourcei(m_source, AL_BUFFER, 0);
    alDeleteSources(1, &m_source);

    // Deactivation of the context
    alcMakeContextCurrent(nullptr);

    // Destruction of the context
    alcDestroyContext(m_context);

    // Closing the device
    alcCloseDevice(m_device);
}

void Audio::playBeep() const {
    ALint state = 0;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        // Sound playback
        alSourcePlay(m_source);
    }
}

void Audio::stopAudio() const {
    ALint state = 0;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        alSourceStop(m_source);
    }
}

void Audio::createBuffer() {
    constexpr auto freq = 400.0f;
    constexpr auto seconds = 1;
    constexpr auto sampleRate = 44100;
    constexpr auto buffSize = seconds * sampleRate;
    std::array<short, buffSize> samples;

    for (int i = 0; i < buffSize; i++)
    {
        auto wave = static_cast<short>(32760 * sin((2.0f * M_PI * freq) / sampleRate * i));
        samples[i] = (short)(wave > 0 ? 32760 : -32760);
    }

    // Filling with samples
    alBufferData(m_buffer, AL_FORMAT_MONO16, samples.data(), buffSize, sampleRate);
}