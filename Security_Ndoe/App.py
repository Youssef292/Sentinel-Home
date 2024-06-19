import sim800l
import time

# Serial Port Configuration
PORT = "/dev/ttyS0"  # Common serial port for SIM800L
BAUDRATE = 9600
phone_number = "+201552229799"  # Replace with the desired number
timeout=15 # Timeout for AT command responses (in seconds)

# Initialize SIM800L
try:
    sim800l = sim800l.SIM800L(PORT, BAUDRATE, timeout=timeout)
    if not sim800l.setup():
        raise Exception("SIM800L initialization failed")
except Exception as e:
    print(f"Error initializing SIM800L: {e}")
    exit(1)


# Make the Call
def make_call():
    print(f"Calling {phone_number}...")
    if sim800l.call(phone_number):
        print("Call initiated successfully")
        input("Press Enter to end the call...")  
        sim800l.hangup()
    else:
        print("Failed to initiate the call")

if __name__ == "__main__":
    make_call()

