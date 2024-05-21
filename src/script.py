import paho.mqtt.client as paho
import time
import ssl
import random
import string
import requests
from ultraSonicSensorFunctions import process_ultrasonic_message, post_or_update_ultrasonic_entity, check_and_create_ultrasonic_subscription

current_light = "yellow"
color_buffer = [] 

# MQTT Settings
broker = "test.mosquitto.org"
port = 8883
topic_light = "/traffic/light"
topic_sensor = "/sensor/ultrasonic"

# Keyrock/OAuth Settings
keyrock_url = 'http://localhost:3005'
client_id = 'bc443a96-01e6-4531-bb60-6a439d74f9b3'
client_secret = 'e67962c8-074e-4926-bede-e23b39340c39'
username = 'admin@test.com'
password = '1234'

def generate_random_id(length=16):
    """Generate a random string of letters and digits."""
    letters_and_digits = string.ascii_letters + string.digits
    return ''.join(random.choice(letters_and_digits) for i in range(length))

def get_oauth_token(client_id, client_secret, username, password):
    print("Retrieve OAuth token from Keyrock.")
    url = f'{keyrock_url}/oauth2/token'
    payload = {
        'grant_type': 'password',
        'username': username,
        'password': password
    }
    auth = (client_id, client_secret)
    response = requests.post(url, data=payload, auth=auth)
    response.raise_for_status()
    return response.json().get('access_token')

def check_for_consistent_color(new_light, client, distance):
    global current_light, oauth_token, color_buffer
    if(len(color_buffer) == 0):
        check_and_create_ultrasonic_subscription()
    color_buffer.append(new_light)

    if len(color_buffer) > 3:
        color_buffer.pop(0)
    print("Buffer:", color_buffer)

    # Check if the last three entries are the same and different from the current light
    if len(set(color_buffer)) == 1 and color_buffer[0] != current_light and len(color_buffer) == 3:
        current_light = color_buffer[0]
        client.publish(topic_light, current_light)
        print("Light updated to:", current_light)
        entity = process_ultrasonic_message(distance, current_light)
        post_or_update_ultrasonic_entity(entity, oauth_token)
        

def on_connect_subscriber(client, userdata, flags, rc):
    client.subscribe(topic_sensor, 0)

def on_message_subscriber(client, userdata, message):
    if message.topic == topic_sensor:
        distance = float(message.payload.decode())
        if distance > 3300 or distance == 0.0:
            new_light = "yellow"
        elif distance < 50:
            new_light = "red"
        else:
            new_light = "green"

        check_for_consistent_color(new_light, client, distance)
    else:
        print("Unknown topic:", message.topic)

def main():
    global oauth_token
    try:
        oauth_token = get_oauth_token(client_id, client_secret, username, password)
    except requests.ConnectionError:
        print("Failed to connect to Keyrock. Please ensure the Keyrock server is running.")
        return

    uid_subscriber = generate_random_id(16)
    client_subscriber = paho.Client(uid_subscriber)
    client_subscriber.on_connect = on_connect_subscriber
    client_subscriber.on_message = on_message_subscriber
    client_subscriber.tls_set(cert_reqs=ssl.CERT_NONE)
    client_subscriber.tls_insecure_set(True)
    client_subscriber.connect(broker, port, 60)
    client_subscriber.loop_start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        client_subscriber.loop_stop()

if __name__ == "__main__":
    main()
