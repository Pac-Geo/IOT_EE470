-- phpMyAdmin SQL Dump
-- version 5.2.2
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1:3306
-- Generation Time: Oct 20, 2026 at 04:26 AM
-- Server version: 11.8.9-MariaDB-log
-- PHP Version: 7.2.34

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `u854844498_GeovaniPalomec`
--

-- --------------------------------------------------------

--
-- Table structure for table `Book`
--

CREATE TABLE `Book` (
  `id` int(11) NOT NULL,
  `name` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `Book`
--

INSERT INTO `Book` (`id`, `name`) VALUES
(1, 'MariaDB Book1a'),
(2, 'MariaDB Book2'),
(3, 'MariaDB Book3'),
(4, 'MariaDB Book4'),
(5, 'MariaDB Book5');

-- --------------------------------------------------------

--
-- Table structure for table `Book2`
--

CREATE TABLE `Book2` (
  `id` int(11) NOT NULL,
  `name` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `Book2`
--

INSERT INTO `Book2` (`id`, `name`) VALUES
(1, 'MariaDB Book1'),
(2, 'MariaDB Book2'),
(3, 'MariaDB Book3'),
(4, 'MariaDB Book4'),
(5, 'MariaDB Book5'),
(6, 'MariaDB Book6');

-- --------------------------------------------------------

--
-- Stand-in structure for view `ensor_combined`
-- (See below for the actual view)
--
CREATE TABLE `ensor_combined` (
`node_name` varchar(10)
,`manufacturer` varchar(10)
,`time_received` datetime
,`temperature` decimal(6,2)
,`humidity` decimal(6,2)
,`longitude` decimal(15,10)
,`latitude` decimal(15,10)
);

-- --------------------------------------------------------

--
-- Table structure for table `Price`
--

CREATE TABLE `Price` (
  `id` int(11) NOT NULL,
  `price` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `Price`
--

INSERT INTO `Price` (`id`, `price`) VALUES
(1, 250),
(2, 252),
(3, 223),
(4, 194),
(5, 305);

-- --------------------------------------------------------

--
-- Table structure for table `Price2`
--

CREATE TABLE `Price2` (
  `id` int(11) NOT NULL,
  `price` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `Price2`
--

INSERT INTO `Price2` (`id`, `price`) VALUES
(6, 251);

-- --------------------------------------------------------

--
-- Table structure for table `product`
--

CREATE TABLE `product` (
  `product_id` int(11) NOT NULL,
  `product_name` varchar(200) DEFAULT NULL,
  `price` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `product`
--

INSERT INTO `product` (`product_id`, `product_name`, `price`) VALUES
(1, 'Desk chair', 50);

-- --------------------------------------------------------

--
-- Table structure for table `sensor_data`
--

CREATE TABLE `sensor_data` (
  `node_name` varchar(10) NOT NULL,
  `time_received` datetime NOT NULL,
  `temperature` decimal(6,2) DEFAULT NULL CHECK (`temperature` between -10 and 100),
  `humidity` decimal(6,2) DEFAULT NULL CHECK (`humidity` between 0 and 100)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `sensor_data`
--

INSERT INTO `sensor_data` (`node_name`, `time_received`, `temperature`, `humidity`) VALUES
('node_1', '2022-10-01 11:00:00', 24.50, 55.00),
('node_1', '2022-10-01 11:30:00', 25.00, 54.00),
('node_1', '2022-10-01 12:00:00', 25.40, 53.00),
('node_1', '2022-10-01 12:30:00', 25.90, 52.00),
('node_1', '2022-10-01 14:00:00', 76.00, 50.00),
('node_1', '2022-10-01 14:30:00', -9.00, 20.00),
('node_2', '2022-10-01 11:00:00', 25.10, 58.00),
('node_2', '2022-10-01 11:30:00', 25.60, 57.00),
('node_2', '2022-10-01 12:00:00', 26.00, 56.00),
('node_2', '2022-10-01 12:30:00', 26.40, 55.00),
('node_3', '2022-10-01 11:00:00', 23.80, 60.00),
('node_3', '2022-10-01 11:30:00', 24.20, 59.00),
('node_3', '2022-10-01 12:00:00', 24.70, 58.00),
('node_3', '2022-10-01 12:30:00', 25.10, 57.00),
('node_4', '2022-10-01 11:00:00', 26.20, 52.00),
('node_4', '2022-10-01 11:30:00', 26.50, 51.00),
('node_4', '2022-10-01 12:00:00', 26.90, 50.00),
('node_4', '2022-10-01 12:30:00', 27.20, 49.00),
('node_5', '2022-10-01 11:00:00', 24.90, 57.00),
('node_5', '2022-10-01 11:30:00', 25.30, 56.00),
('node_5', '2022-10-01 12:00:00', 25.80, 55.00),
('node_5', '2022-10-01 12:30:00', 26.20, 54.00),
('node_5', '2022-10-01 14:30:00', -1.00, 100.00);

-- --------------------------------------------------------

--
-- Table structure for table `sensor_register`
--

CREATE TABLE `sensor_register` (
  `node_name` varchar(10) NOT NULL,
  `manufacturer` varchar(10) DEFAULT NULL,
  `longitude` decimal(15,10) DEFAULT NULL,
  `latitude` decimal(15,10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `sensor_register`
--

INSERT INTO `sensor_register` (`node_name`, `manufacturer`, `longitude`, `latitude`) VALUES
('node_1', 'Espressif', -99.0107070000, 19.2751750000),
('node_2', 'Bosch', -99.0107070000, 19.2751750000),
('node_3', 'Honeywell', -99.0107070000, 19.2751750000),
('node_4', 'Sensirion', -99.0107070000, 19.2751750000),
('node_5', 'Siemens', -99.0107070000, 19.2751750000);

-- --------------------------------------------------------

--
-- Table structure for table `t2`
--

CREATE TABLE `t2` (
  `name` varchar(30) DEFAULT NULL CHECK (char_length(`name`) > 2),
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL CHECK (`start_date` is null or `end_date` is null or `start_date` < `end_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `t2`
--

INSERT INTO `t2` (`name`, `start_date`, `end_date`) VALUES
('Ione', '2003-12-15', '2014-11-09'),
('Ione', NULL, '2014-11-09');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `Book`
--
ALTER TABLE `Book`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `Book2`
--
ALTER TABLE `Book2`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `Price`
--
ALTER TABLE `Price`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `Price2`
--
ALTER TABLE `Price2`
  ADD KEY `id` (`id`);

--
-- Indexes for table `product`
--
ALTER TABLE `product`
  ADD PRIMARY KEY (`product_id`);

--
-- Indexes for table `sensor_data`
--
ALTER TABLE `sensor_data`
  ADD PRIMARY KEY (`node_name`,`time_received`);

--
-- Indexes for table `sensor_register`
--
ALTER TABLE `sensor_register`
  ADD PRIMARY KEY (`node_name`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `Book`
--
ALTER TABLE `Book`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `Book2`
--
ALTER TABLE `Book2`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT for table `Price`
--
ALTER TABLE `Price`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `product`
--
ALTER TABLE `product`
  MODIFY `product_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

-- --------------------------------------------------------

--
-- Structure for view `ensor_combined`
--
DROP TABLE IF EXISTS `ensor_combined`;

CREATE ALGORITHM=UNDEFINED DEFINER=`u854844498_db_GeovaniP`@`127.0.0.1` SQL SECURITY DEFINER VIEW `ensor_combined`  AS SELECT `d`.`node_name` AS `node_name`, `r`.`manufacturer` AS `manufacturer`, `d`.`time_received` AS `time_received`, `d`.`temperature` AS `temperature`, `d`.`humidity` AS `humidity`, `r`.`longitude` AS `longitude`, `r`.`latitude` AS `latitude` FROM (`sensor_data` `d` join `sensor_register` `r` on(`d`.`node_name` = `r`.`node_name`)) ;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `Price2`
--
ALTER TABLE `Price2`
  ADD CONSTRAINT `Price2_ibfk_1` FOREIGN KEY (`id`) REFERENCES `Book2` (`id`);

--
-- Constraints for table `sensor_data`
--
ALTER TABLE `sensor_data`
  ADD CONSTRAINT `fk_sensor_node` FOREIGN KEY (`node_name`) REFERENCES `sensor_register` (`node_name`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
