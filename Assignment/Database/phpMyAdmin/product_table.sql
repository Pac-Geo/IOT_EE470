-- Create product table with auto-incrementing ID
CREATE TABLE product (
    product_id INT AUTO_INCREMENT PRIMARY KEY,
    product_name VARCHAR(200),
    price INT
);

-- Insert sample product
INSERT INTO product (product_name, price)
VALUES ('Desk chair', 50);
