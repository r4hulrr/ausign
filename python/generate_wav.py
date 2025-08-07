from gtts import gTTS
from pydub import AudioSegment
import os

# Config
text = "Name"
volume_boost_db = 10
output_dir = "sounds/"
wav_filename = "name.wav"
wav_path = os.path.join(output_dir, wav_filename)

# Make sure output directory exists
os.makedirs(output_dir, exist_ok=True)

# Generate speech using gTTS
tts = gTTS(text, lang='en')
tts.save("temp.mp3")

# Load and process audio
audio = AudioSegment.from_mp3("temp.mp3")
processed = (
    audio + volume_boost_db
).set_frame_rate(44100).set_channels(2).set_sample_width(2)  # 16-bit stereo 44.1kHz

# Export as WAV
processed.export(wav_path, format="wav")

print(f"Saved WAV audio to: {wav_path} ({os.path.getsize(wav_path)} bytes)")
