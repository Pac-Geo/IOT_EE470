-- Create Book table
CREATE TABLE Book (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

-- Create Price table
CREATE TABLE Price (
    id INT NOT NULL AUTO_INCREMENT,
    price FLOAT NOT NULL,
    PRIMARY KEY (id)
);

-- Insert books
INSERT INTO Book (id, name)
VALUES
    (1, 'MariaDB Book1a'),
    (2, 'MariaDB Book2'),
    (3, 'MariaDB Book3'),
    (4, 'MariaDB Book4'),
    (5, 'MariaDB Book5');

-- Insert corresponding prices
INSERT INTO Price (id, price)
VALUES
    (1, 250),
    (2, 252),
    (3, 223),
    (4, 194),
    (5, 305);
