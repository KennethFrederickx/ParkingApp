import datetime
import requests

ORION_URL = "http://localhost:1026/ngsi-ld/v1/entities"
NGSI_LD_CONTEXT = "https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context.jsonld"

def process_ultrasonic_message(distance, light_color):
    print(distance);
    print("received message:", str(distance) + "cm" + " ")

    ngsi_entity = {
        "id": "urn:ngsi-ld:UltraSonicSensorReading:1",
        "type": "SensorReading",
        "@context": NGSI_LD_CONTEXT,
        "distance": {
            "type": "Property",
            "value": distance,
        },
        "light": {
            "type": "Property",
            "value": light_color,
        },
    }
    return ngsi_entity


def post_or_update_ultrasonic_entity(entity, oauth_token):
    headers = {
        "Content-Type": "application/ld+json",
        'Authorization': f'Bearer {oauth_token}'
    }

    response = requests.patch(f"{ORION_URL}/{entity['id']}/attrs", headers=headers, json=entity)
    if response.status_code in [200, 204]:
        print(f"Entity {entity['id']} updated:", response.status_code, response.text)

    if response.status_code == 404:
        response = requests.post(ORION_URL, headers=headers, json=entity)
        print(f"Entity {entity['id']} posted:", response.status_code, response.text)        
    
    return response.status_code




def create_ultrasonic_subscription():
    headers = {
        "Content-Type": "application/ld+json",
    }
    subscription = {
        "id": "urn:ngsi-ld:Subscription:3",
        "description": "Notify QuantumLeap",
        "type": "Subscription",
        "entities": [
            {
                "type": "SensorReading"
            }
        ],
        "watchedAttributes": ["distance"],
        "notification": {
            "attributes": ["distance"],
            "endpoint": {
                "uri": "http://quantumleap:8668/v2/notify",
                "accept": "application/json",
            }
        },
        "@context": NGSI_LD_CONTEXT,        
    }

    response = requests.post("http://localhost:1026/ngsi-ld/v1/subscriptions", headers=headers, json=subscription)
    if response.status_code == 201:
        print("Subscription created successfully.")
    else:
        print("Failed to create subscription:", response.status_code, response.text)


def check_and_create_ultrasonic_subscription():
    response = requests.get("http://localhost:1026/ngsi-ld/v1/subscriptions/urn:ngsi-ld:Subscription:3")
    if response.status_code == 404:
        print("No subscription found, creating one...")
        create_ultrasonic_subscription()
    else:
        print("Subscription already exists.")