-- Таблица секций
CREATE TABLE IF NOT EXISTS sections (
    section_id INT PRIMARY KEY AUTO_INCREMENT,
    section_name VARCHAR(255) NOT NULL,
    trainer VARCHAR(255) NOT NULL,
    description_ TEXT NOT NULL,
    max_students INT NOT NULL,
    current_students INT DEFAULT 0
) ENGINE=InnoDB;

-- Таблица пользователей
CREATE TABLE IF NOT EXISTS users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    email VARCHAR(255) NOT NULL UNIQUE,
    pass_number VARCHAR(20) NOT NULL UNIQUE,
    issued_by VARCHAR(255) NOT NULL,
    date_of_issue DATE NOT NULL,
    student_ticket VARCHAR(50),
    password VARCHAR(255) NOT NULL,
    full_name VARCHAR(255) NOT NULL,
    role ENUM('student', 'secretary') NOT NULL,
    section_enrolled INT,
    FOREIGN KEY (section_enrolled) REFERENCES sections(section_id) ON DELETE SET NULL
) ENGINE=InnoDB;

-- Таблица заявок
CREATE TABLE IF NOT EXISTS applications (
    application_id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT NOT NULL,
    section_id INT NOT NULL,
    status ENUM('pending', 'accepted', 'paid', 'rejected') NOT NULL DEFAULT 'pending',
    application_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (section_id) REFERENCES sections(section_id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Таблица сессий
CREATE TABLE IF NOT EXISTS user_sessions (
    session_id VARCHAR(128) PRIMARY KEY,
    user_id INT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB;