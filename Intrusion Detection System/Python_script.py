import serial
import json
import time
import csv
import smtplib
from email.message import EmailMessage
import tkinter as tk
import threading
from datetime import datetime


# ========================= CONFIGURATION =========================
SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600


DISTANCE_THRESHOLD = 20      # cm
VIBRATION_THRESHOLD = 400    # analog value (0â€“1023)


CSV_FILE = "intrusions.csv"


# --- Email Configuration ---
EMAIL_CONFIG = {
    "smtp_server": "smtp.gmail.com",
    "smtp_port": 587,
    "username": "abhishek4755.se24@chitkara.edu.in",       # your email
    "password": "wicw ihkj rwxj qslh",     # Gmail App Password or SendGrid API key
    "to": "abhishekmohindra1106@gmail.com",
    "from": "amitmohindra02@gmail.com"
}



# Buzzer GPIO on Raspberry Pi (BCM numbering)
BUZZ_PIN = 18   # change if you used different pin
# =================================================================

# ---------- Setup GPIO ----------
try:
    import RPi.GPIO as GPIO
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(BUZZ_PIN, GPIO.OUT)
    # prepare PWM object but do not start
    try:
        pwm = GPIO.PWM(BUZZ_PIN, 2000)  # 2kHz default tone
    except Exception:
        pwm = None
    GPIO_available = True
except Exception:
    print("[!] GPIO not available. Buzzer will be simulated.")
    GPIO = None
    pwm = None
    GPIO_available = False

# ---------- Utilities ----------
def safe_buzz_seconds(seconds=5, freq=2000):
    """
    Produce a tone for `seconds`. For passive buzzer we use PWM (preferred).
    If PWM not available, toggle GPIO HIGH/LOW as fallback.
    """
    if GPIO_available and pwm:
        try:
            pwm.ChangeFrequency(freq)
            pwm.start(50)  # 50% duty
            time.sleep(seconds)
            pwm.stop()
            return
        except Exception as e:
            print("[!] PWM failed:", e)

    if GPIO_available:
        try:
            GPIO.output(BUZZ_PIN, GPIO.HIGH)
            time.sleep(seconds)
            GPIO.output(BUZZ_PIN, GPIO.LOW)
            return
        except Exception as e:
            print("[!] GPIO buzz fallback failed:", e)

    # simulated
    print(f"[BUZZER simulated for {seconds}s at {freq}Hz]")

def send_email(subject, body):
    msg = EmailMessage()
    msg["From"] = EMAIL_CONFIG["username"]
    msg["To"] = EMAIL_CONFIG["to"]
    msg["Subject"] = subject
    msg.set_content(body)
    try:
        with smtplib.SMTP(EMAIL_CONFIG["smtp_server"], EMAIL_CONFIG["smtp_port"], timeout=10) as smtp:
            smtp.ehlo()
            smtp.starttls()
            smtp.login(EMAIL_CONFIG["username"], EMAIL_CONFIG["password"])
            smtp.send_message(msg)
        print("[+] Email sent successfully.")
    except Exception as e:
        print("[!] Failed to send email:", e)

def show_popup(text="âš ï¸ INTRUSION DETECTED âš ï¸", duration=5):
    def _popup():
        root = tk.Tk()
        root.title("ALERT")
        root.geometry("420x180")
        root.attributes("-topmost", True)
        # simple centered label
        tk.Label(root, text=text, font=("Arial", 16, "bold"), fg="white", bg="red").pack(expand=True, fill="both")
        root.after(duration * 1000, root.destroy)
        root.mainloop()
    threading.Thread(target=_popup, daemon=True).start()

def log_event(event_type, distance, vibration):
    # event_type: "someone_at_door" or "intrusion"
    hdr_needed = False
    try:
        with open(CSV_FILE, "r"):
            pass
    except FileNotFoundError:
        hdr_needed = True

    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        if hdr_needed:
            writer.writerow(["timestamp", "event_type", "distance_cm", "vibration_value"])
        writer.writerow([datetime.now().isoformat(), event_type, distance, vibration])
    print(f"[+] Logged event: {event_type}")

# ---------- Main ----------
def main():
    print("[*] Starting intrusion_detector_modes")
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    except Exception as e:
        print("[!] Cannot open serial port:", e)
        return
    print("[*] Listening on", SERIAL_PORT)

    last_someone_time = 0
    last_intrusion_time = 0
    SOMEONE_DEBOUNCE = 20     # seconds before sending another "someone at door" mail
    INTRUSION_DEBOUNCE = 30   # seconds before repeating full intrusion

    while True:
        try:
            raw = ser.readline().decode("utf-8", errors="ignore").strip()
            if not raw:
                continue
            # Expect JSON like {"distance":12,"vibration":512}
            try:
                data = json.loads(raw)
            except json.JSONDecodeError:
                # Could be partial garbage; skip
                continue

            distance = data.get("distance", 999)
            vibration = data.get("vibration", 0)
            print(f"Distance: {distance} cm | Vibration: {vibration}")

            now = time.time()
            ultra_trigger = (distance < DISTANCE_THRESHOLD)
            vib_trigger = (vibration > VIBRATION_THRESHOLD)

            # Full intrusion: both triggers
            if ultra_trigger and vib_trigger:
                if now - last_intrusion_time >= INTRUSION_DEBOUNCE:
                    last_intrusion_time = now
                    print("âš ï¸ Intrusion detected (fusion). Executing full alarm.")
                    log_event("intrusion", distance, vibration)
                    # buzz 5s (non-blocking) -> run in thread so email/popup can start immediately
                    threading.Thread(target=safe_buzz_seconds, args=(5, 2000), daemon=True).start()
                    show_popup("âš ï¸ INTRUSION DETECTED âš ï¸", duration=5)
                    subject = "P-IDS ALERT: Intrusion Detected"
                    body = (f"Intrusion detected at {datetime.now().isoformat()}.\n"
                            f"Distance: {distance} cm\nVibration: {vibration}\n")
                    threading.Thread(target=send_email, args=(subject, body), daemon=True).start()
                else:
                    print("[*] Intrusion detected but debounced; skipping repeated alarm.")

            # Only ultrasonic (someone at door)
            elif ultra_trigger and not vib_trigger:
                if now - last_someone_time >= SOMEONE_DEBOUNCE:
                    last_someone_time = now
                    print("â„¹ï¸ Someone at the door (ultrasonic only). Sending notification.")
                    log_event("someone_at_door", distance, vibration)
                    subject = "P-IDS: Someone at the door"
                    body = (f"Someone is detected at the door at {datetime.now().isoformat()}.\n"
                            f"Distance: {distance} cm\nVibration: {vibration}\n")
                    threading.Thread(target=send_email, args=(subject, body), daemon=True).start()
                else:
                    print("[*] Someone-at-door event debounced; skipping repeated notification.")

            # else: no event
            time.sleep(0.05)

        except KeyboardInterrupt:
            print("\n[!] Exiting...")
            try:
                if pwm:
                    pwm.stop()
                if GPIO:
                    GPIO.cleanup()
            except Exception:
                pass
            break
        except Exception as e:
            print("[!] Runtime error:", e)
            time.sleep(0.5)

if __name__ == "__main__":
    main()






