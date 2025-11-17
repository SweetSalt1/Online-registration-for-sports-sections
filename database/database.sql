-- Таблица секций
CREATE TABLE sections (
    section_id INT PRIMARY KEY AUTO_INCREMENT, -- уникальный id секции
    section_name VARCHAR(255) NOT NULL, -- название секции
    trainer VARCHAR(255) NOT NULL, -- фио тренера
    description_ TEXT NOT NULL, -- описание секции (преподавателя записываем либо сюда, либо создаём отдельный столбец)
    max_students INT NOT NULL, -- максимальное количество студентов в секции
    current_students INT DEFAULT 0 -- текущее количество записанных студентов
);
-- Таблица пользователей
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT, -- Уникальный id пользователя (будет генерироваться автоматически)
    --phone VARCHAR(20) NOT NULL UNIQUE, -- номер телефона
    email VARCHAR(255) NOT NULL UNIQUE, -- почта
    pass_number VARCHAR(20) NOT NULL UNIQUE, -- серия и номер паспорта
    issued_by VARCHAR(255) NOT NULL, -- кем выдан
    date_of_issue DATE NOT NULL, -- дата выдачи (время типа 26.10.2025)
    student_ticket VARCHAR(50) NULL, -- номер студенческого билета (только для студентов)
    password VARCHAR(255) NOT NULL, -- пароль
    full_name VARCHAR(255) NOT NULL, -- ФИО (студента или секретаря)
    role ENUM('student', 'secretary') NOT NULL, -- здесь определяется, чей аккаунт, студента или секретаря
    --created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- дата регистрации аккаунта (так же создаётся автоматически)
    section_enrolled INT NULL, -- id секции, на которую записан студент
    FOREIGN KEY (section_enrolled) REFERENCES sections(section_id) ON DELETE SET NULL -- Внешний ключ
);
-- Таблица заявок
CREATE TABLE applications (
    application_id INT PRIMARY KEY AUTO_INCREMENT, -- уникальный id для каждой заявки
    student_id INT NOT NULL, -- здесь id студента, который отправил заявку (этот id берётся из таблицы пользователей)
    section_id INT NOT NULL, -- здесь id секции, на которую студент подал заявку (из таблицы секций)
    status ENUM('pending', 'accepted', 'paid', 'rejected') NOT NULL, -- статус заявки:
    -- under_consideration - рассматривается
    -- accepted - принято(требуется оплата)
    -- approved - одобрено(записан на секцию)
    -- no_places - мест нет
    application_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- дата заявки
    -- Ниже внешние ключи - это связи между таблицами.
    FOREIGN KEY (student_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (section_id) REFERENCES sections(section_id) ON DELETE CASCADE
);
-- таблица сессий
CREATE TABLE user_sessions (
    session_id VARCHAR(128) PRIMARY KEY, -- уникальный id сессии (НЕ ГЕНЕРИРУЕТСЯ автоматически)
    user_id INT NOT NULL, -- id пользователя
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,  -- дата создания сессии (чтобы можно было удалять сессию которым больше например 30 дней)
    -- внешние ключи
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE 
);
