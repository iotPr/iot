import os
from pydub import AudioSegment
import speech_recognition as sr

def convert_wav_to_text(file_path):
    # Create a recognizer instance
    recognizer = sr.Recognizer()

    # Load the WAV file
    audio = AudioSegment.from_wav(file_path)

    # Save the WAV file in the required format for the recognizer
    temp_wav_path = "temp.wav"
    audio.export(temp_wav_path, format="wav")

    # Open the file using speech_recognition
    with sr.AudioFile(temp_wav_path) as source:
        audio_data = recognizer.record(source)

        # Try to recognize the speech in the audio
        try:
            text = recognizer.recognize_google(audio_data)
            print("Recognized text:", text)
        except sr.UnknownValueError:
            print("Google Speech Recognition could not understand audio")
        except sr.RequestError as e:
            print(f"Could not request results from Google Speech Recognition service; {e}")

    # Remove the temporary file
    os.remove(temp_wav_path)

if __name__ == "__main__":
    # Replace 'your_file.wav' with the path to your WAV file
    file_path = "src\\OSR_us_000_0010_8k.wav"
    convert_wav_to_text(file_path)