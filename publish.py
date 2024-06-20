from paho.mqtt import client as mqtt_client
import time
import ssl

# MQTT broker credentials
Brocker = "a3aea2a70f7b43d1809561231ab50b37.s1.eu.hivemq.cloud"
port = 8883
client_id = "ESP8266Client1"
username = "ESP32"
password = "123456aA"

# Define the MQTT topics
Main_Room_Light_topic = "Main_Room"
Personal_Room_topic = "Personal_Room"
Garage_topic = "Garage"
Outside_topic = "Outside"
window_control_topic = "window_control"

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to the Broker")
        else:
            print(f"Failed to connect with code {rc}")

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.tls_set(ca_certs=None, certfile=None, keyfile=None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1_2)

    return client

def publish(client, topic, status):
    msg = status
    result = client.publish(topic, msg)
    msg_status = result.rc
    if msg_status == 0:
        print(f"Message: {msg} sent to topic {topic}")
    else:
        print(f"Failed to send message to topic {topic}")

def main():
    client = connect_mqtt()
    client.connect(Brocker, port)
    client.loop_start()  # Add this line to start the message loop in the background

    try:
        while True:
            print("Choose a topic to send message to:")
            print("1. Main Room Light")
            print("2. Personal Room")
            print("3. Garage")
            print("4. Outside")
            print("5. Window Control")
            print("6. Exit")
            choice = input("Enter your choice: ")

            if choice == '1':
                status = input("Enter Main Room Light status (on/off): ")
                publish(client, Main_Room_Light_topic, status)
            elif choice == '2':
                status = input("Enter Personal Room status (on/off): ")
                publish(client, Personal_Room_topic, status)
            elif choice == '3':
                status = input("Enter Garage status (on/off): ")
                publish(client, Garage_topic, status)
            elif choice == '4':
                status = input("Enter Outside status (on/off): ")
                publish(client, Outside_topic, status)
            elif choice == '5':
                status = input("Enter Window Control status (open/close): ")
                publish(client, window_control_topic, status)
            elif choice == '6':
                break
            else:
                print("Invalid choice. Please enter a valid option.")

            time.sleep(2)  # Extend the sleep duration to allow the loop to handle messages
            
    except KeyboardInterrupt:
        print("Exiting...")
        client.loop_stop()  # Stop the background loop
        client.disconnect()

if __name__ == '__main__':
    main()

