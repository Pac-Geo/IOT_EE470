-- Display all tables
SHOW TABLES;

-- Display the combined sensor data
SELECT *
FROM ensor_combined
ORDER BY node_name, time_received;

-- Count measurements from each sensor
SELECT
    node_name,
    COUNT(*) AS number_of_readings
FROM sensor_data
GROUP BY node_name
ORDER BY node_name;
