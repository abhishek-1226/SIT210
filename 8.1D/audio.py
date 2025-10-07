import speech_recognition as sr
import RPi.GPIO as GPIO
import threading
import time

LED_PIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.output(LED_PIN, GPIO.LOW)

led_status = False

def update_led(status):
    global led_status
    led_status = status
    GPIO.output(LED_PIN, GPIO.HIGH if status else GPIO.LOW)
    print(f"LED {'ON' if status else 'OFF'}")

recognizer = sr.Recognizer()
microphone = sr.Microphone()

def listen_for_commands():
    with microphone as source:
        recognizer.adjust_for_ambient_noise(source)
        print("Listening for voice command...")
        try:
            audio = recognizer.listen(source, timeout=2, phrase_time_limit=5)
            command = recognizer.recognize_google(audio).lower()
            print(f"You said: {command}")
            return command
        except sr.WaitTimeoutError:
            return None
        except sr.UnknownValueError:
            print("Could not understand audio")
            return None
        except sr.RequestError as e:
            print(f"Speech recognition error: {e}")
            return None

def process_command(command):
    if any(phrase in command for phrase in ["light on", "switch on"]):
        update_led(True)
    elif any(phrase in command for phrase in ["light off", "switch off"]):
        update_led(False)
    else:
        print("Command not recognized")

def continuous_listening():
    while True:
        command = listen_for_commands()
        if command:
            process_command(command)
        time.sleep(0.5)

try:
    listening_thread = threading.Thread(target=continuous_listening, daemon=True)
    listening_thread.start()

    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print("\nExiting program...")

finally:
    GPIO.cleanup()
    print("GPIO cleaned up.")