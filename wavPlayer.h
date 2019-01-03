#include "FS.h"
#include "driver/i2s.h"

typedef struct {
  uint32_t ChunkID;
  uint32_t ChunkSize;
  uint32_t Format;

  uint32_t Subchunk1ID;
  uint32_t Subchunk1Size;
  uint16_t AudioFormat;
  uint16_t NumChannels;//
  uint32_t SampleRate;//
  uint32_t ByteRate;
  uint16_t BlockAlign;
  uint16_t BitsPerSample;//

  uint32_t Subchunk2ID;
  uint32_t Subchunk2Size;
} wavProperties_t;

/* variables hold file, state of process wav file and wav file properties */
File root;
wavProperties_t wavProps;

//i2s configuration
int i2s_num = 0; // i2s port number
static const i2s_config_t i2s_config = {
     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
     .sample_rate = 44100,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = I2S_COMM_FORMAT_PCM,
     .intr_alloc_flags = 0, // default interrupt priority
     .dma_buf_count = 8,
     .dma_buf_len = 64,
     .use_apll = false
};

void playWAVData(File root) {

  if (root.available()) {

    root.read((uint8_t *)&wavProps, sizeof(wavProperties_t));

    if (wavProps.ChunkID == (*(uint32_t *)"RIFF")
        && wavProps.Format == (*(uint32_t *)"WAVE")
        && wavProps.Subchunk2ID == (*(uint32_t *)"data")) {
      Serial.println("WAV_DATA:");
      Serial.println(wavProps.NumChannels);
      Serial.println(wavProps.SampleRate);
      Serial.println(wavProps.BitsPerSample);

      //i2s_config.sample_rate = (int)wavProps.SampleRate;
      //i2s_config.bits_per_sample = (i2s_bits_per_sample_t)wavProps.BitsPerSample;
      i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
      i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
      //set sample rates of i2s to sample rate of wav file
      //i2s_set_sample_rates((i2s_port_t)i2s_num, wavProps.SampleRate);
      //i2s_set_clk((i2s_port_t)i2s_num, wavProps.SampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
      Serial.println("WAV Prepared!");
      uint16_t sample;
      for (uint32_t i = 0; i < wavProps.Subchunk2Size/2; i++) {
        root.read((uint8_t *)&sample, sizeof(sample));
        i2s_write_bytes((i2s_port_t)i2s_num, &sample, sizeof(sample), 100);
        i2s_write_bytes((i2s_port_t)i2s_num, &sample, sizeof(uint8_t), 100);
      }
      root.close();
      i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver
      Serial.println("done!");

    } else {
      Serial.println("WAV_ERROR");
      return;
    }

  }
}
