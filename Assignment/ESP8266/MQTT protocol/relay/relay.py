import getpass
import json
import os
import re
import urllib.error
import urllib.request
from datetime import datetime
from pathlib import Path

import paho.mqtt.client as mqtt


BROKER_HOST = "broker.hivemq.com"
BROKER_PORT = 8000
MQTT_TOPIC = "testtopic/temp/outTopic/EE470pacgeo8266/potentiometer"
KEEPALIVE_SECONDS = 60


def load_local_env() -> None:
    env_file = Path(__file__).with_name(".env")
    if not env_file.is_file():
        return

    for line in env_file.read_text(encoding="utf-8").splitlines():
        entry = line.strip()
        if not entry or entry.startswith("#") or "=" not in entry:
            continue
        name, value = entry.split("=", 1)
        os.environ.setdefault(name.strip(), value.strip())


def save_reading(value: int, api_url: str, api_token: str) -> None:
    body = json.dumps({"value": value}).encode("utf-8")
    request = urllib.request.Request(
        api_url,
        data=body,
        headers={
            "Content-Type": "application/json",
            "X-Relay-Token": api_token,
        },
        method="POST",
    )

    try:
        with urllib.request.urlopen(request, timeout=10) as response:
            if not 200 <= response.status < 300:
                print(f"Database API returned HTTP {response.status}.")
                return
            print("Reading saved to the database.")
    except urllib.error.HTTPError as error:
        print(f"Database API rejected the reading (HTTP {error.code}).")
    except urllib.error.URLError as error:
        print(f"Could not reach the database API: {error.reason}")
    except TimeoutError:
        print("Database API request timed out.")


def main() -> None:
    load_local_env()
    api_url = os.environ.get("MQTT_RELAY_API_URL", "").strip()
    if not api_url.startswith("https://"):
        raise SystemExit(
            "Set MQTT_RELAY_API_URL to the HTTPS URL of record_potentiometer.php."
        )

    api_token = os.environ.get("MQTT_RELAY_API_TOKEN", "").strip()
    if not api_token:
        api_token = getpass.getpass("Relay API token: ").strip()
    if not api_token:
        raise SystemExit("The relay API token cannot be empty.")

    def on_connect(client, userdata, flags, reason_code, properties):
        if reason_code == 0:
            print(f"Connected to {BROKER_HOST}.")
            result, _ = client.subscribe(MQTT_TOPIC)
            if result == mqtt.MQTT_ERR_SUCCESS:
                print(f"Subscribed to {MQTT_TOPIC}")
            else:
                print(f"Could not subscribe to {MQTT_TOPIC} (code {result}).")
        else:
            print(f"MQTT connection failed (reason code {reason_code}).")

    def on_message(client, userdata, message):
        try:
            payload = message.payload.decode("ascii").strip()
        except UnicodeDecodeError:
            print("Ignored a non-ASCII potentiometer message.")
            return

        if re.fullmatch(r"\d{1,4}", payload) is None:
            print(f"Ignored invalid potentiometer payload: {payload!r}")
            return

        value = int(payload)
        if not 0 <= value <= 1023:
            print(f"Ignored out-of-range potentiometer value: {value}")
            return

        received_at = datetime.now().astimezone().isoformat(timespec="seconds")
        print(f"Received {value} from {message.topic} at {received_at}")
        save_reading(value, api_url, api_token)

    client = mqtt.Client(
        callback_api_version=mqtt.CallbackAPIVersion.VERSION2,
        transport="websockets",
    )
    client.on_connect = on_connect
    client.on_message = on_message

    print(f"Connecting to {BROKER_HOST}:{BROKER_PORT} over WebSockets...")
    client.connect(BROKER_HOST, BROKER_PORT, KEEPALIVE_SECONDS)
    print("Listening for potentiometer readings. Press Ctrl+C to stop.")
    client.loop_forever()


if __name__ == "__main__":
    main()
