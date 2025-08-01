from gtts import gTTS
from pydub import AudioSegment
import os

# config
text = "This is a test of loud audio from ESP32."
volume_boost_db = 10
output_dir = "code/data"
raw_filename = "loud_audio.raw"
raw_path = os.path.join(output_dir, raw_filename)

# tts
tts = gTTS(text, lang='en')
tts.save("temp.mp3")

# load mp3 and process
audio = AudioSegment.from_mp3("temp.mp3")
processed = (
    audio + volume_boost_db
).set_frame_rate(8000).set_channels(1).set_sample_width(1)  # 8-bit, mono, 8kHz

# export as pcm
with open(raw_path, 'wb') as f:
    f.write(processed.raw_data)

print(f"Saved RAW audio to: {raw_path} ({len(processed.raw_data)} bytes)")
