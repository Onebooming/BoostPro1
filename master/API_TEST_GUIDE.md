# BoostPro API 测试用例文档

## 概述

本文档描述了 BoostPro 教育管理系统的三个主要 API 端点的测试用例：
- `/api/student` - 学生管理API
- `/api/teacher` - 教师管理API
- `/api/course` - 课程管理API

**服务器地址**: `http://127.0.0.1:20080`

## 快速开始

### 1. 启动服务器

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### 2. 运行自动化测试脚本

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master
./test_api.sh
```

---

## 一、Student API (学生管理)

### 基础信息
- **端点**: `/api/student`
- **支持方法**: GET, POST
- **注意**: 当前不支持 PUT (更新) 和 DELETE (删除)

### 1.1 查询所有学生

**请求**:
```bash
curl -X GET http://127.0.0.1:20080/api/student
```

**响应示例**:
```json
[
  {
    "id": "1",
    "name": "张三",
    "birth_date": "2000-01-01",
    "address": "北京市",
    "gender": "男",
    "hobby": "编程",
    "phone_number": "13800138000",
    "father_name": "父亲",
    "mother_name": "母亲",
    "grade": "2024",
    "class_name": "计算机1班",
    "major": "计算机科学与技术",
    "student_id": "S20240001"
  }
]
```

### 1.2 按ID查询学生

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/student?id=1"
```

**参数**:
- `id` (string): 学生的数据库ID

**响应示例**:
```json
{
  "id": "1",
  "name": "张三",
  "birth_date": "2000-01-01"
}
```

### 1.3 按姓名查询学生

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/student?name=张"
```

**参数**:
- `name` (string): 学生姓名（支持模糊匹配）

**响应示例**:
```json
[
  {
    "id": "1",
    "name": "张三",
    "birth_date": "2000-01-01"
  }
]
```

### 1.4 添加学生

**请求**:
```bash
curl -X POST http://127.0.0.1:20080/api/student \
  -H "Content-Type: application/json" \
  -d '{
    "name": "李四",
    "birth_date": "2001-05-15",
    "address": "上海市",
    "gender": "男",
    "hobby": "篮球",
    "phone_number": "13900139000",
    "father_name": "父亲",
    "mother_name": "母亲",
    "grade": "2024",
    "class_name": "计算机2班",
    "major": "软件工程",
    "student_id": "S20240002"
  }'
```

**请求字段**:
| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| name | string | 是 | 学生姓名 |
| birth_date | string | 否 | 出生日期 |
| address | string | 否 | 地址 |
| gender | string | 否 | 性别（男/女） |
| hobby | string | 否 | 爱好 |
| phone_number | string | 否 | 电话号码 |
| father_name | string | 否 | 父亲姓名 |
| mother_name | string | 否 | 母亲姓名 |
| grade | string | 否 | 年级 |
| class_name | string | 否 | 班级 |
| major | string | 否 | 专业 |
| student_id | string | 是 | 学号 |

**响应示例**:
```json
{
  "result": "student added"
}
```

---

## 二、Teacher API (教师管理)

### 基础信息
- **端点**: `/api/teacher`
- **支持方法**: GET, POST, PUT, DELETE

### 2.1 查询所有教师

**请求**:
```bash
curl -X GET http://127.0.0.1:20080/api/teacher
```

**响应示例**:
```json
[
  {
    "id": "1",
    "name": "王教授",
    "birth_date": "1980-05-15",
    "address": "北京市",
    "gender": "男",
    "hobby": "科研",
    "phone_number": "13800138001",
    "father_name": "父亲",
    "mother_name": "母亲",
    "department": "计算机学院",
    "position": "教授",
    "office": "A101",
    "teacher_id": "T20240001",
    "courses_taught": []
  }
]
```

### 2.2 按ID查询教师

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/teacher?id=1"
```

**参数**:
- `id` (string): 教师的数据库ID

### 2.3 按姓名查询教师

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/teacher?name=王"
```

**参数**:
- `name` (string): 教师姓名（支持模糊匹配）

### 2.4 添加教师

**请求**:
```bash
curl -X POST http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{
    "name": "李教授",
    "birth_date": "1985-08-20",
    "address": "上海市",
    "gender": "男",
    "hobby": "阅读",
    "phone_number": "13900139001",
    "father_name": "父亲",
    "mother_name": "母亲",
    "department": "软件学院",
    "position": "副教授",
    "office": "B202",
    "teacher_id": "T20240002"
  }'
```

**请求字段**:
| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| name | string | 是 | 教师姓名 |
| birth_date | string | 否 | 出生日期 |
| address | string | 否 | 地址 |
| gender | string | 否 | 性别（男/女） |
| hobby | string | 否 | 爱好 |
| phone_number | string | 否 | 电话号码 |
| father_name | string | 否 | 父亲姓名 |
| mother_name | string | 否 | 母亲姓名 |
| department | string | 否 | 部门 |
| position | string | 否 | 职位 |
| office | string | 否 | 办公室 |
| teacher_id | string | 是 | 教师工号 |

**响应示例**:
```json
{
  "result": "teacher added"
}
```

### 2.5 更新教师

**请求**:
```bash
curl -X PUT http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{
    "id": "1",
    "name": "王教授",
    "birth_date": "1980-05-15",
    "address": "北京市海淀区",
    "gender": "男",
    "department": "计算机学院",
    "position": "正教授",
    "office": "A301",
    "teacher_id": "T20240001"
  }'
```

**参数**:
- `id` (string): 必须提供，指定要更新的教师ID

**响应示例**:
```json
{
  "result": "teacher updated"
}
```

### 2.6 删除教师

**请求**:
```bash
curl -X DELETE "http://127.0.0.1:20080/api/teacher?id=1"
```

**参数**:
- `id` (string): 要删除的教师ID

**响应示例**:
```json
{
  "result": "teacher deleted"
}
```

或

```json
{
  "error": "teacher not found or delete failed"
}
```

---

## 三、Course API (课程管理)

### 基础信息
- **端点**: `/api/course`
- **支持方法**: GET, POST, PUT, DELETE

### 3.1 查询所有课程

**请求**:
```bash
curl -X GET http://127.0.0.1:20080/api/course
```

**响应示例**:
```json
[
  {
    "id": "1",
    "course_id": "C20240001",
    "course_name": "数据结构",
    "teacher_id": "T20240001",
    "teacher_name": "王教授",
    "credits": 3.0,
    "max_students": 100,
    "current_students": 45,
    "course_type": "必修",
    "semester": "2024-春季",
    "description": "计算机专业核心课程",
    "created_at": "2024-01-01 10:00:00",
    "updated_at": "2024-01-01 10:00:00",
    "schedules": [
      {
        "day_of_week": "周一",
        "start_time": "08:00",
        "end_time": "09:40",
        "location": "A101",
        "week_range": "1-16周"
      }
    ]
  }
]
```

### 3.2 按ID查询课程

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/course?id=1"
```

**参数**:
- `id` (string): 课程的数据库ID

### 3.3 按名称查询课程

**请求**:
```bash
curl -X GET "http://127.0.0.1:20080/api/course?name=数据结构"
```

**参数**:
- `name` (string): 课程名称（支持模糊匹配）

### 3.4 添加课程

**请求**:
```bash
curl -X POST http://127.0.0.1:20080/api/course \
  -H "Content-Type: application/json" \
  -d '{
    "course_id": "C20240002",
    "course_name": "算法设计",
    "teacher_id": "T20240001",
    "credits": 4.0,
    "max_students": 80,
    "current_students": 0,
    "course_type": "必修",
    "semester": "2024-春季",
    "description": "算法设计与分析课程"
  }'
```

**请求字段**:
| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| course_id | string | 是 | 课程编号 |
| course_name | string | 是 | 课程名称 |
| teacher_id | string | 是 | 授课教师ID |
| credits | double | 否 | 学分 |
| max_students | int | 否 | 最大选课人数 |
| current_students | int | 否 | 当前选课人数 |
| course_type | string | 否 | 课程类型（必修/选修/专业选修） |
| semester | string | 否 | 学期 |
| description | string | 否 | 课程描述 |

**响应示例**:
```json
{
  "result": "course added"
}
```

### 3.5 更新课程

**请求**:
```bash
curl -X PUT http://127.0.0.1:20080/api/course \
  -H "Content-Type: application/json" \
  -d '{
    "id": "1",
    "course_id": "C20240001",
    "course_name": "数据结构与算法",
    "teacher_id": "T20240001",
    "credits": 4.0,
    "max_students": 120,
    "current_students": 45,
    "course_type": "必修",
    "semester": "2024-春季",
    "description": "更新后的课程描述"
  }'
```

**参数**:
- `id` (string): 必须提供，指定要更新的课程ID

**响应示例**:
```json
{
  "result": "course updated"
}
```

### 3.6 删除课程

**请求**:
```bash
curl -X DELETE "http://127.0.0.1:20080/api/course?id=1"
```

**参数**:
- `id` (string): 要删除的课程ID

**响应示例**:
```json
{
  "result": "course deleted"
}
```

或

```json
{
  "error": "course not found or delete failed"
}
```

---

## 四、错误响应

所有API在出错时返回如下格式的错误信息：

```json
{
  "error": "错误描述信息"
}
```

常见错误：
- `"invalid json"` - JSON格式错误
- `"Method not found"` - 不支持的方法
- `"Course not found"` - 课程不存在
- `"Teacher not found"` - 教师不存在
- `"missing xxx id"` - 缺少必要的ID参数

---

## 五、测试命令清单

### Student API
```bash
# 查询所有
curl -X GET http://127.0.0.1:20080/api/student

# 按ID查询
curl -X GET "http://127.0.0.1:20080/api/student?id=1"

# 按姓名查询
curl -X GET "http://127.0.0.1:20080/api/student?name=张"

# 添加学生
curl -X POST http://127.0.0.1:20080/api/student \
  -H "Content-Type: application/json" \
  -d '{"name":"测试学生","student_id":"S001","gender":"男"}'
```

### Teacher API
```bash
# 查询所有
curl -X GET http://127.0.0.1:20080/api/teacher

# 按ID查询
curl -X GET "http://127.0.0.1:20080/api/teacher?id=1"

# 按姓名查询
curl -X GET "http://127.0.0.1:20080/api/teacher?name=王"

# 添加教师
curl -X POST http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{"name":"测试教师","teacher_id":"T001","department":"计算机学院"}'

# 更新教师
curl -X PUT http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{"id":"1","name":"更新后的教师","teacher_id":"T001"}'

# 删除教师
curl -X DELETE "http://127.0.0.1:20080/api/teacher?id=1"
```

### Course API
```bash
# 查询所有
curl -X GET http://127.0.0.1:20080/api/course

# 按ID查询
curl -X GET "http://127.0.0.1:20080/api/course?id=1"

# 按名称查询
curl -X GET "http://127.0.0.1:20080/api/course?name=数据结构"

# 添加课程
curl -X POST http://127.0.0.1:20080/api/course \
  -H "Content-Type: application/json" \
  -d '{"course_id":"C001","course_name":"测试课程","teacher_id":"T001"}'

# 更新课程
curl -X PUT http://127.0.0.1:20080/api/course \
  -H "Content-Type: application/json" \
  -d '{"id":"1","course_id":"C001","course_name":"更新后的课程","teacher_id":"T001"}'

# 删除课程
curl -X DELETE "http://127.0.0.1:20080/api/course?id=1"
```

---

## 六、注意事项

1. **服务器必须先启动** - 确保在运行测试前 `boost_server` 已启动
2. **数据库连接** - 确保MySQL数据库服务运行且连接配置正确
3. **ID字段** - 更新和删除操作需要提供正确的 `id` 字段（雪花ID）
4. **JSON格式** - POST和PUT请求必须使用正确的JSON格式
5. **字符编码** - 建议使用UTF-8编码处理中文内容

---

## 七、故障排查

### 连接被拒绝
```bash
Error: Connection refused
```
**解决方案**: 检查服务器是否启动
```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### JSON解析错误
```json
{"error": "invalid json"}
```
**解决方案**: 检查请求体是否为有效JSON，使用工具验证
```bash
echo '{"name":"test"}' | python3 -m json.tool
```

### 记录不存在
```json
{"error": "Course not found"}
```
**解决方案**: 确认提供的ID存在，或先查询所有记录获取正确ID
