import serial
import time

# Serial port configuration
SERIAL_PORT = '/dev/ttyUSB0'  # Serial port on Raspberry Pi
BAUD_RATE = 9600

# Function to send AT command and read response
def send_at_command(command, timeout=1):
    ser.write((command + '\r\n').encode())
    time.sleep(timeout)
    response = ser.read(ser.in_waiting).decode().strip()
    print(f"Command: {command}, Response: {response}")  # Debugging output
    return response

# Connect to SIM800L module
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print("Established communication with", ser.name)
except Exception as e:
    print("Error connecting to SIM800L:", e)
    exit(1)

# Function to make a call
def make_call(phone_number):
    try:
        response = send_at_command(f'ATD{phone_number};')
        if 'OK' in response:
            print(f"Calling {phone_number}...")
        else:
            print("Failed to initiate call.")
    except Exception as e:
        print("Failed to initiate call:", e)

# Function to send an SMS
def send_sms(phone_number, message):
    try:
        send_at_command(f'AT+CMGS="{phone_number}"')
        send_at_command(message)
        send_at_command(chr(26))  # End of message character
        print("SMS sent successfully.")
    except Exception as e:
        print("Failed to send SMS:", e)
