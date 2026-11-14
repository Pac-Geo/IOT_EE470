CREATE TABLE potentiometer_data (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    adc_value SMALLINT UNSIGNED NOT NULL,
    time_received TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    INDEX idx_potentiometer_time (time_received),
    CONSTRAINT chk_potentiometer_adc CHECK (adc_value BETWEEN 0 AND 1023)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
