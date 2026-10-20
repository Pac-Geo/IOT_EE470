-- Create second Book table
CREATE TABLE Book2 (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
) ENGINE=InnoDB;

-- Create second Price table with foreign key
CREATE TABLE Price2 (
    id INT NOT NULL,
    price FLOAT NOT NULL,
    FOREIGN KEY (id) REFERENCES Book2(id)
) ENGINE=InnoDB;

-- Register books
INSERT INTO Book2 (id, name)
VALUES
    (1, 'MariaDB Book1'),
    (2, 'MariaDB Book2'),
    (3, 'MariaDB Book3'),
    (4, 'MariaDB Book4'),
    (5, 'MariaDB Book5'),
    (6, 'MariaDB Book6');

-- Insert a price linked to a registered book
INSERT INTO Price2 (id, price)
VALUES (6, 251);
