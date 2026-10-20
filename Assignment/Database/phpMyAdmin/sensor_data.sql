-- Create the sensor measurement table
CREATE TABLE sensor_data (
    node_name VARCHAR(10) PRIMARY KEY,
    time_received DATETIME,
    temperature DECIMAL(6,2)
        CHECK (temperature BETWEEN -10 AND 100),
    humidity DECIMAL(6,2)
        CHECK (humidity BETWEEN 0 AND 100)
) ENGINE=InnoDB;

-- Allow multiple measurements per node,
-- but prevent duplicate timestamps for the same node.
-- Reject measurements from unregistered sensors.
ALTER TABLE sensor_data
    MODIFY time_received DATETIME NOT NULL,
    DROP PRIMARY KEY,
    ADD PRIMARY KEY (node_name, time_received),
    ADD CONSTRAINT fk_sensor_node
        FOREIGN KEY (node_name)
        REFERENCES sensor_register(node_name);
