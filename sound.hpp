#pragma once
#include <filesystem>
#include <miniaudio/miniaudio.h>

class sound_test {
public:
    sound_test() = default;
    sound_test(const std::string& p_path);
    ~sound_test();

    void play();
    void stop();

    void cleanup();

    [[nodiscard]] bool loaded_file() const { return m_file_loaded; }

private:
    bool m_file_loaded=false;
    ma_decoder m_decoder;
    ma_device m_audio_device_handler;
    ma_device_config m_audio_device_config;
};