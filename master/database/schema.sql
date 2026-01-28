-- BoostPro 教育管理系统数据库架构
-- 创建数据库（如果不存在）
CREATE DATABASE IF NOT EXISTS boostpro1 CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE boostpro1;

-- 删除已存在的表（谨慎使用）
-- DROP TABLE IF EXISTS enrollment;
-- DROP TABLE IF EXISTS grades;
-- DROP TABLE IF EXISTS courses;
-- DROP TABLE IF EXISTS teachers;
-- DROP TABLE IF EXISTS students;

-- 1. 学生表（已存在，这里提供完整的结构）
CREATE TABLE IF NOT EXISTS students (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    name VARCHAR(100) NOT NULL COMMENT '学生姓名',
    birth_date VARCHAR(20) DEFAULT NULL COMMENT '出生日期',
    address VARCHAR(200) DEFAULT NULL COMMENT '家庭住址',
    gender ENUM('男', '女') DEFAULT NULL COMMENT '性别',
    hobby VARCHAR(200) DEFAULT NULL COMMENT '兴趣爱好',
    phone_number VARCHAR(20) DEFAULT NULL COMMENT '联系电话',
    father_name VARCHAR(100) DEFAULT NULL COMMENT '父亲姓名',
    mother_name VARCHAR(100) DEFAULT NULL COMMENT '母亲姓名',
    grade VARCHAR(20) DEFAULT NULL COMMENT '年级',
    class_name VARCHAR(50) DEFAULT NULL COMMENT '班级',
    major VARCHAR(100) DEFAULT NULL COMMENT '专业',
    student_id VARCHAR(50) UNIQUE NOT NULL COMMENT '学号，业务唯一标识',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    INDEX idx_student_id (student_id),
    INDEX idx_name (name),
    INDEX idx_grade_class (grade, class_name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='学生信息表';

-- 2. 教师表
CREATE TABLE IF NOT EXISTS teachers (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    name VARCHAR(100) NOT NULL COMMENT '教师姓名',
    birth_date VARCHAR(20) DEFAULT NULL COMMENT '出生日期',
    address VARCHAR(200) DEFAULT NULL COMMENT '家庭住址',
    gender ENUM('男', '女') DEFAULT NULL COMMENT '性别',
    hobby VARCHAR(200) DEFAULT NULL COMMENT '兴趣爱好',
    phone_number VARCHAR(20) DEFAULT NULL COMMENT '联系电话',
    father_name VARCHAR(100) DEFAULT NULL COMMENT '父亲姓名',
    mother_name VARCHAR(100) DEFAULT NULL COMMENT '母亲姓名',
    department VARCHAR(100) DEFAULT NULL COMMENT '所属院系',
    position VARCHAR(100) DEFAULT NULL COMMENT '职称',
    office VARCHAR(100) DEFAULT NULL COMMENT '办公室位置',
    teacher_id VARCHAR(50) UNIQUE NOT NULL COMMENT '教师工号，业务唯一标识',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    INDEX idx_teacher_id (teacher_id),
    INDEX idx_name (name),
    INDEX idx_department (department)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='教师信息表';

-- 3. 课程表
CREATE TABLE IF NOT EXISTS courses (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    course_id VARCHAR(50) UNIQUE NOT NULL COMMENT '课程编号，业务唯一标识',
    course_name VARCHAR(200) NOT NULL COMMENT '课程名称',
    teacher_id VARCHAR(64) NOT NULL COMMENT '授课教师ID',
    credits DECIMAL(3,1) DEFAULT 0.0 COMMENT '学分',
    max_students INT DEFAULT 0 COMMENT '最大选课人数',
    current_students INT DEFAULT 0 COMMENT '当前选课人数',
    course_type ENUM('必修', '选修', '专业选修') DEFAULT '选修' COMMENT '课程类型',
    semester VARCHAR(20) DEFAULT NULL COMMENT '学期（如：2024-春季）',
    description TEXT DEFAULT NULL COMMENT '课程描述',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    FOREIGN KEY (teacher_id) REFERENCES teachers(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_course_id (course_id),
    INDEX idx_course_name (course_name),
    INDEX idx_teacher (teacher_id),
    INDEX idx_semester (semester)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='课程信息表';

-- 4. 课程时间表
CREATE TABLE IF NOT EXISTS course_schedules (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    course_id VARCHAR(64) NOT NULL COMMENT '课程ID',
    day_of_week ENUM('星期一', '星期二', '星期三', '星期四', '星期五', '星期六', '星期日') NOT NULL COMMENT '星期几',
    start_time TIME NOT NULL COMMENT '开始时间',
    end_time TIME NOT NULL COMMENT '结束时间',
    location VARCHAR(100) DEFAULT NULL COMMENT '上课地点',
    week_range VARCHAR(50) DEFAULT NULL COMMENT '周次范围（如：1-16周）',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_course (course_id),
    INDEX idx_time_slot (day_of_week, start_time, end_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='课程时间表';

-- 5. 选课表
CREATE TABLE IF NOT EXISTS enrollment (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    student_id VARCHAR(64) NOT NULL COMMENT '学生ID',
    course_id VARCHAR(64) NOT NULL COMMENT '课程ID',
    enrollment_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '选课时间',
    status ENUM('已选课', '已退课', '已完成') DEFAULT '已选课' COMMENT '选课状态',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE ON UPDATE CASCADE,
    UNIQUE KEY unique_enrollment (student_id, course_id),
    INDEX idx_student (student_id),
    INDEX idx_course (course_id),
    INDEX idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='学生选课表';

-- 6. 成绩表
CREATE TABLE IF NOT EXISTS grades (
    id VARCHAR(64) PRIMARY KEY COMMENT '雪花ID，全局唯一标识符',
    student_id VARCHAR(64) NOT NULL COMMENT '学生ID',
    course_id VARCHAR(64) NOT NULL COMMENT '课程ID',
    enrollment_id VARCHAR(64) NOT NULL COMMENT '选课记录ID',
    regular_score DECIMAL(5,2) DEFAULT NULL COMMENT '平时成绩（0-100）',
    midterm_score DECIMAL(5,2) DEFAULT NULL COMMENT '期中成绩（0-100）',
    final_score DECIMAL(5,2) DEFAULT NULL COMMENT '期末成绩（0-100）',
    total_score DECIMAL(5,2) DEFAULT NULL COMMENT '总成绩（0-100）',
    gpa DECIMAL(3,2) DEFAULT NULL COMMENT '绩点（0-4.0）',
    grade_level ENUM('A+', 'A', 'A-', 'B+', 'B', 'B-', 'C+', 'C', 'C-', 'D', 'F') DEFAULT NULL COMMENT '等级评定',
    remarks TEXT DEFAULT NULL COMMENT '备注',
    graded_by VARCHAR(64) DEFAULT NULL COMMENT '评分教师ID',
    graded_at TIMESTAMP DEFAULT NULL COMMENT '评分时间',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (enrollment_id) REFERENCES enrollment(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (graded_by) REFERENCES teachers(id) ON DELETE SET NULL ON UPDATE CASCADE,
    UNIQUE KEY unique_grade (student_id, course_id),
    INDEX idx_student (student_id),
    INDEX idx_course (course_id),
    INDEX idx_enrollment (enrollment_id),
    INDEX idx_total_score (total_score),
    INDEX idx_grade_level (grade_level)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='学生成绩表';

-- 7. 数据库触发器：自动更新课程的当前选课人数
DELIMITER //
CREATE TRIGGER IF NOT EXISTS update_course_students_after_enrollment
AFTER INSERT ON enrollment
FOR EACH ROW
BEGIN
    UPDATE courses
    SET current_students = (
        SELECT COUNT(*) FROM enrollment
        WHERE course_id = NEW.course_id AND status = '已选课'
    )
    WHERE id = NEW.course_id;
END//
DELIMITER ;

DELIMITER //
CREATE TRIGGER IF NOT EXISTS update_course_students_after_enrollment_update
AFTER UPDATE ON enrollment
FOR EACH ROW
BEGIN
    -- 更新旧课程
    UPDATE courses
    SET current_students = (
        SELECT COUNT(*) FROM enrollment
        WHERE course_id = OLD.course_id AND status = '已选课'
    )
    WHERE id = OLD.course_id;

    -- 更新新课程（如果课程ID发生变化）
    IF NEW.course_id != OLD.course_id THEN
        UPDATE courses
        SET current_students = (
            SELECT COUNT(*) FROM enrollment
            WHERE course_id = NEW.course_id AND status = '已选课'
        )
        WHERE id = NEW.course_id;
    END IF;
END//
DELIMITER ;

DELIMITER //
CREATE TRIGGER IF NOT EXISTS update_course_students_after_enrollment_delete
AFTER DELETE ON enrollment
FOR EACH ROW
BEGIN
    UPDATE courses
    SET current_students = (
        SELECT COUNT(*) FROM enrollment
        WHERE course_id = OLD.course_id AND status = '已选课'
    )
    WHERE id = OLD.course_id;
END//
DELIMITER ;

-- 8. 视图：学生课程信息视图
CREATE OR REPLACE VIEW student_course_info AS
SELECT
    s.id as student_id,
    s.student_id as student_no,
    s.name as student_name,
    c.id as course_id,
    c.course_id as course_no,
    c.course_name,
    c.credits,
    c.semester,
    t.name as teacher_name,
    e.status as enrollment_status,
    e.enrollment_time,
    g.total_score,
    g.grade_level,
    g.gpa
FROM students s
JOIN enrollment e ON s.id = e.student_id
JOIN courses c ON e.course_id = c.id
JOIN teachers t ON c.teacher_id = t.id
LEFT JOIN grades g ON e.id = g.enrollment_id;

-- 9. 视图：教师课程统计视图
CREATE OR REPLACE VIEW teacher_course_stats AS
SELECT
    t.id as teacher_id,
    t.teacher_id as teacher_no,
    t.name as teacher_name,
    t.department,
    COUNT(c.id) as total_courses,
    COUNT(e.id) as total_students,
    AVG(g.total_score) as avg_score
FROM teachers t
LEFT JOIN courses c ON t.id = c.teacher_id
LEFT JOIN enrollment e ON c.id = e.course_id AND e.status = '已选课'
LEFT JOIN grades g ON e.id = g.enrollment_id
GROUP BY t.id, t.teacher_id, t.name, t.department;

-- 插入示例数据
INSERT IGNORE INTO students (id, name, student_id, gender, grade, class_name, major, phone_number) VALUES
('2025010100001', '张三', '2024001', '男', '2024级', '计算机1班', '计算机科学与技术', '13800138001'),
('2025010100002', '李四', '2024002', '女', '2024级', '计算机1班', '计算机科学与技术', '13800138002'),
('2025010100003', '王五', '2024003', '男', '2024级', '计算机2班', '计算机科学与技术', '13800138003');

INSERT IGNORE INTO teachers (id, name, teacher_id, gender, department, position, phone_number) VALUES
('2025010200001', '陈教授', 'T1001', '男', '计算机学院', '教授', '13900139001'),
('2025010200002', '刘老师', 'T1002', '女', '计算机学院', '副教授', '13900139002');

INSERT IGNORE INTO courses (id, course_id, course_name, teacher_id, credits, course_type, semester) VALUES
('2025010300001', 'CS101', '计算机导论', '2025010200001', 3.0, '必修', '2024-秋季'),
('2025010300002', 'CS201', '数据结构', '2025010200001', 4.0, '必修', '2024-秋季'),
('2025010300003', 'CS301', '算法设计', '2025010200002', 3.0, '专业选修', '2024-秋季');

-- 插入课程时间表
INSERT IGNORE INTO course_schedules (id, course_id, day_of_week, start_time, end_time, location, week_range) VALUES
('2025010400001', '2025010300001', '星期一', '08:00:00', '09:30:00', '教学楼A101', '1-16周'),
('2025010400002', '2025010300001', '星期三', '08:00:00', '09:30:00', '教学楼A101', '1-16周'),
('2025010400003', '2025010300002', '星期二', '10:00:00', '11:30:00', '教学楼B202', '1-16周'),
('2025010400004', '2025010300002', '星期四', '10:00:00', '11:30:00', '教学楼B202', '1-16周'),
('2025010400005', '2025010300003', '星期五', '14:00:00', '15:30:00', '教学楼C303', '1-16周');

COMMIT;