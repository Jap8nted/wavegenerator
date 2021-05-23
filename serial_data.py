import serial


usb_port = "ttyUSB0"
usb_speed = 115200


not_finished = True
current_measurements = list()
voltage_measurements = list()
with serial.Serial(f"/dev/{usb_port}", usb_speed, timeout=5) as serial_port:
    # Read an array until it receives an end of lin
    while True:
        byte_read = serial_port.read()
        print(byte_read)
        if byte_read == "\n":
            break
    print(f"[INFO] Closing serial port")
exit(0)
