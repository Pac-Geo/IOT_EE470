-- Combine book names and prices using an INNER JOIN
SELECT
    Book.name,
    Price.price
FROM Book
INNER JOIN Price
    ON Book.id = Price.id
ORDER BY Book.id;
