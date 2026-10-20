-- Create a table demonstrating CHECK constraints
CREATE TABLE t2 (
    name VARCHAR(30) CHECK (CHAR_LENGTH(name) > 2),
    start_date DATE,
    end_date DATE CHECK (
        start_date IS NULL
        OR end_date IS NULL
        OR start_date < end_date
    )
);

-- Insert valid records
INSERT INTO t2 (name, start_date, end_date)
VALUES
    ('Ione', '2003-12-15', '2014-11-09'),
    ('Ione', NULL, '2014-11-09');
