-- Create the combined sensor view
CREATE VIEW ensor_combined AS
SELECT
    d.node_name,
    r.manufacturer,
    d.time_received,
    d.temperature,
    d.humidity,
    r.longitude,
    r.latitude
FROM sensor_data AS d
INNER JOIN sensor_register AS r
    ON d.node_name = r.node_name;
