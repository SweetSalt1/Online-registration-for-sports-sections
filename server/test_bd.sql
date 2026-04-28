-- Подготовка тестовых данных
INSERT INTO users (email, password, full_name, role) VALUES ('test@test.com', 'pass', 'Тест Тестов', 'student');
SET @uid = LAST_INSERT_ID();

INSERT INTO sections (section_name, trainer, description, max_students) VALUES ('Тестовая секция', 'Тренер', 'Описание', 10);
SET @sid = LAST_INSERT_ID();


-- Тест 1: UNIQUE email
INSERT INTO users (email, password, full_name, role) VALUES ('test@test.com', 'pass', 'Дубль', 'student');


-- Тест 2: NOT NULL и ENUM role
INSERT INTO users (email, password, full_name) VALUES ('test2@test.com', 'pass', 'Без роли');


-- Тест 3: DEFAULT значения
INSERT INTO applications (student_id, section_id) VALUES (@uid, @sid);
SELECT CASE WHEN status = 'pending' AND application_date IS NOT NULL 
       THEN 'PASS: defaults work' ELSE 'FAIL' END AS test_3 FROM applications 
WHERE student_id = @uid AND section_id = @sid;

INSERT INTO sections (section_name, trainer, description, max_students) VALUES ('Секция 2', 'Тренер 2', 'Описание 2', 5);
SELECT CASE WHEN current_students = 0 THEN 'PASS: current_students default 0' ELSE 'FAIL' END AS test_3b 
FROM sections WHERE section_name = 'Секция 2';


-- Тест 4: FOREIGN KEY (должен упасть с ошибкой)
INSERT INTO applications (student_id, section_id) VALUES (9999, @sid);


-- Тест 5: CASCADE DELETE
DELETE FROM users WHERE user_id = @uid;
SELECT CASE WHEN COUNT(*) = 0 THEN 'PASS: cascade delete works' ELSE 'FAIL' END AS test_5 
FROM applications WHERE student_id = @uid;

