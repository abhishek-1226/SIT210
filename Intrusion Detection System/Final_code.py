import serial
import time
import smtplib
import ssl
import signal
import sys
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# ---------------------- USER CONFIG ----------------------
SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600

EMAIL_SENDER = "abhishek4755.se24@chitkara.edu.in"
EMAIL_PASSWORD = "wicw ihkj rwxj qslh"
EMAIL_RECEIVER = "abhishekmohindra1106@gmail.com"

DIST_THRESHOLD = 20
BUZZ_DURATION = 4
READ_CONFIRM_COUNT = 12
DEBOUNCE_AFTER_ALERT = 5

# ---------------------- STATES ---------------------------
pending_detections = 0
last_alert_time = 0
alert_in_progress = False

# ---------------------- INTERRUPT EXIT --------------------
def handle_exit(signum, frame):
    print("\n[INFO] Exiting cleanly...")
    try:
        ser.close()
    except:
        pass
    sys.exit(0)

signal.signal(signal.SIGINT, handle_exit)

# ---------------------- EMAIL FUNCTION -------------------
def send_email(subject, body):
    global alert_in_progress
    alert_in_progress = True
    print("[EMAIL] Sending alert...")

    msg = MIMEMultipart()
    msg["From"] = EMAIL_SENDER
    msg["To"] = EMAIL_RECEIVER
    msg["Subject"] = subject
    msg.attach(MIMEText(body, "plain"))

    context = ssl.create_default_context()
    try:
        with smtplib.SMTP_SSL("smtp.gmail.com", 465, context=context) as server:
            server.login(EMAIL_SENDER, EMAIL_PASSWORD)
            server.sendmail(EMAIL_SENDER, EMAIL_RECEIVER, msg.as_string())
        print("[EMAIL] Sent successfully.")
    except Exception as e:
        print("[EMAIL ERROR]", e)

    alert_in_progress = False

# ---------------------- SERIAL INIT ----------------------
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)
print("[INFO] System Online. Listening...\n")

# ---------------------- MAIN LOOP ------------------------
while True:

    if alert_in_progress:
        time.sleep(0.2)
        continue

    line = ser.readline().decode().strip()

    if not line.startswith("D:"):
        continue

    try:
        parts = line.split(",")
        distance = int(parts[0].split(":")[1])
        vibration = int(parts[1].split(":")[1])
    except:
        continue

    print(f"Distance: {distance} cm | Vibration: {vibration}")

    # Debounce after alert
    if time.time() - last_alert_time < DEBOUNCE_AFTER_ALERT:
        continue

    intrusion_distance = distance < DIST_THRESHOLD
    intrusion_vibration = vibration == 1

    # ---------------- Intrusion Logic ----------------
    if intrusion_distance and intrusion_vibration:
        print("[!!!] TRUE INTRUSION DETECTED")

        ser.write(b"B")          # tell Arduino to buzz
        time.sleep(0.1)          # small pause so command transmits cleanly

        send_email("?? INTRUSION ALERT!", "Forced entry attempt detected at your door!")

        last_alert_time = time.time()
        pending_detections = 0
        continue

    # ---------------- Person Near Door ----------------
    if intrusion_distance:
        pending_detections += 1
        if pending_detections >= READ_CONFIRM_COUNT:
            print("[INFO] Person detected near door")

            send_email("Person Detected", "Someone is standing at your door.")
            last_alert_time = time.time()
            pending_detections = 0
    else:
        pending_detections = 0

    time.sleep(0.1)
