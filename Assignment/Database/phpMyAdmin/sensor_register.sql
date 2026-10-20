-- Store registered sensors and their locations
CREATE TABLE sensor_register (
    node_name VARCHAR(10) PRIMARY KEY,
    manufacturer VARCHAR(10),
    longitude DECIMAL(15,10),
    latitude DECIMAL(15,10)
) ENGINE=InnoDB;

-- Register five different sensor nodes
INSERT INTO sensor_register
    (node_name, manufacturer, longitude, latitude)
VALUES
    ('node_1', 'Espressif', -99.010707, 19.275175),
    ('node_2', 'Bosch',     -99.010707, 19.275175),
    ('node_3', 'Honeywell', -99.010707, 19.275175),
    ('node_4', 'Sensirion', -99.010707, 19.275175),
    ('node_5', 'Siemens',   -99.010707, 19.275175);
