from flask import Flask, Response
import serial
import serial.tools.list_ports

app = Flask(__name__)

# --- Find available ports ---
ports = [p.device for p in serial.tools.list_ports.comports()]
print("Available ports:", ports)

ser = None
target_port = "/dev/ttyUSB0"
baud = 57600

if target_port in ports:
    try:
        ser = serial.Serial(target_port, baud)
        print(f"Opened {target_port} at {baud} baud")
    except serial.SerialException as e:
        print(f"Failed to open {target_port}: {e}")
else:
    print(f"{target_port} not found. Available: {ports}")

@app.route("/serial")
def serial_data():
    def generate():
        if ser is None:
            yield "data: ERROR - No serial device connected\n\n"
            return
        while True:
            try:
                line = ser.readline().decode(errors="ignore").strip()
                yield f"data: {line}\n\n"
            except Exception as e:
                yield f"data: ERROR - {e}\n\n"
                break
    return Response(generate(), mimetype="text/event-stream")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
