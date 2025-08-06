from gtts import gTTS
from pydub import AudioSegment
import os

# Config
text = "love"
volume_boost_db = 10
output_dir = "code/data"
raw_filename = "love.raw"
raw_path = os.path.join(output_dir, raw_filename)

# Generate speech
tts = gTTS(text, lang='en')
tts.save("temp.mp3")

# Process audio to match ESP32 I2S config
audio = AudioSegment.from_mp3("temp.mp3")
processed = (
    audio + volume_boost_db
).set_frame_rate(44100).set_channels(2).set_sample_width(2)  # 16-bit, stereo, 44.1kHz

# Export as raw PCM
with open(raw_path, 'wb') as f:
    f.write(processed.raw_data)

print(f"Saved RAW audio to: {raw_path} ({len(processed.raw_data)} bytes)")
