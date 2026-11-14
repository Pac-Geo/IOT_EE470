# MQTT potentiometer relay

The ESP publishes one potentiometer ADC value when its button is pressed. The
Python relay subscribes to that topic, prints each valid value, and sends it
to the Hostinger PHP API. The API stores it with a database timestamp, and the
graph page displays the latest 200 readings.

## 1. Set up the database and PHP files

1. Run `database/potentiometer_data.sql` in the Hostinger database.
2. Upload the contents of `web/` to a folder such as `public_html/mqtt/`,
   including the local `db_config.php`. That file is excluded from Git. Check
   its database settings against the current Hostinger credentials before
   uploading; the local copy was initialized from the existing repository
   configuration and may be outdated.
3. Keep the same random `relay_token` in `web/db_config.php` and
   `relay/.env`. Both local files are excluded from Git.
4. Open `potentiometer_graph.html` on the hosted site to verify the page loads.

Use the Hostinger database credentials from your hosting control panel, not
credentials committed in other repository files. Rotate any database password
that has been committed to a public repository.

## 2. Run the Python relay

From this `relay/` directory:

```sh
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
```

The local `relay/.env` contains the API URL and matching relay token. Check
that the URL matches where you uploaded the PHP files, then run:

```sh
python relay.py
```

The relay listens on the public HiveMQ WebSocket endpoint at
`broker.hivemq.com:8000` and subscribes to
`testtopic/temp/outTopic/EE470pacgeo8266/potentiometer`. It accepts only
integer ADC values from 0 through 1023. It does not publish to MQTT.

## Remote MQTT controls and readings

The public HiveMQ broker is shared, so remote MQTT clients can use the same
broker without your database credentials:

- Subscribe to `testtopic/temp/outTopic/EE470pacgeo8266` for the ESP button's
  `1` and delayed `0` messages.
- Subscribe to
  `testtopic/temp/outTopic/EE470pacgeo8266/potentiometer` for potentiometer
  ADC readings.
- Publish the single-character payload `1` or `0` to
  `testtopic/temp/inTopic` to turn the ESP LED on or off.

## 3. Test the complete path

1. Start the relay and wait for its subscription confirmation.
2. Confirm the ESP is connected to Wi-Fi and HiveMQ.
3. Press the ESP button once. The ESP publishes one potentiometer reading.
4. Confirm the relay prints the value and reports a successful database insert.
5. Reload `potentiometer_graph.html` to see the timestamped point.

The HiveMQ public broker is shared and unauthenticated. Topic names are not
private, so the relay validates payloads and the PHP endpoint requires a
separate token before accepting database writes.
