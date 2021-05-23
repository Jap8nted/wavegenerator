import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient
from pydantic import BaseModel, ValidationError
from typing import Dict, List, Optional
import json
import pprint

MQTT_ADDRESS = "127.0.0.1"
MQTT_PORT = 1883
MQTT_USER = "japonted"
MQTT_PASSWORD = "japonted"
MQTT_TOPIC = "home/energy"
MQTT_CLIENT_ID = "power_meter"


class EnergyMeterStationData(BaseModel):
    current: List[int]
    voltage: List[int]
    sensor_id: int
    sensor_phase: int


def on_connect(client, userdata, flags, rc):
    client.subscribe(MQTT_TOPIC)


def _parse_mqtt_payload(payload):
    return json.loads(str(payload.decode("utf-8", "ignore")))


def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")
    #data = _parse_mqtt_message(msg.topic, msg.payload)
    data = str(msg.payload.decode("utf-8", "ignore"))
    print(data)
    # if data:
    #    pprint.pprint(data)


def _parse_mqtt_message(topic, payload):
    if topic == MQTT_TOPIC:
        payload_dict = _parse_mqtt_payload(payload)
        try:
            data = EnergyMeterStationData(**payload_dict)
            return data
        except ValidationError as e:
            print(e)
            return None


def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, MQTT_PORT)
    mqtt_client.loop_forever()


if __name__ == "__main__":
    print("Starting to process the messages")
    main()
