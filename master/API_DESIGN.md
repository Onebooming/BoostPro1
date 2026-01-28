# BoostPro 教育管理系统 - RESTful API 设计文档

## 版本信息
- **版本**: v1.0.0
- **基础路径**: `/api/v1`
- **协议**: HTTP/HTTPS
- **数据格式**: JSON

## 目录
- [1. 设计原则](#1-设计原则)
- [2. 统一响应格式](#2-统一响应格式)
- [3. 接口规范](#3-接口规范)
- [4. API端点](#4-api端点)
- [5. 错误处理](#5-错误处理)
- [6. 分页排序过滤](#6-分页排序过滤)
- [7. 认证授权](#7-认证授权)

---

## 1. 设计原则

### 1.1 RESTful原则
- **资源导向**: URL表示资源，HTTP方法表示操作
- **无状态**: 每个请求包含所有必要信息
- **统一接口**: 使用标准的HTTP方法和状态码
- **分层系统**: 支持代理、网关等中间层

### 1.2 命名规范
- **URL**: 使用小写字母，单词用连字符分隔
- **资源名**: 使用复数形式 `/api/v1/students`
- **参数名**: 使用蛇形命名法 `user_id`, `created_at`

### 1.3 HTTP方法映射
```
GET    /api/v1/resources      - 获取资源列表
GET    /api/v1/resources/{id} - 获取单个资源
POST   /api/v1/resources      - 创建资源
PUT    /api/v1/resources/{id} - 更新整个资源
PATCH  /api/v1/resources/{id} - 部分更新资源
DELETE /api/v1/resources/{id} - 删除资源
```

---

## 2. 统一响应格式

### 2.1 成功响应

```json
{
  "code": 200,
  "message": "success",
  "data": {
    "id": "123",
    "name": "张三"
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

### 2.2 列表响应（带分页）

```json
{
  "code": 200,
  "message": "success",
  "data": {
    "items": [
      {"id": "1", "name": "张三"},
      {"id": "2", "name": "李四"}
    ],
    "pagination": {
      "page": 1,
      "page_size": 20,
      "total": 100,
      "total_pages": 5
    }
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

### 2.3 错误响应

```json
{
  "code": 400,
  "message": "Bad Request",
  "error": {
    "type": "ValidationError",
    "details": "姓名字段不能为空",
    "field": "name"
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

---

## 3. 接口规范

### 3.1 URL设计

#### 基础URL
```
http://[server]:[port]/api/v1/[resource]
```

#### 资源层级
```
/api/v1/students                    # 学生列表
/api/v1/students/{id}               # 单个学生
/api/v1/students/{id}/courses       # 学生的课程
/api/v1/teachers                    # 教师列表
/api/v1/teachers/{id}               # 单个教师
/api/v1/teachers/{id}/courses       # 教师的课程
/api/v1/courses                     # 课程列表
/api/v1/courses/{id}                # 单个课程
/api/v1/courses/{id}/students       # 课程的学生（选课）
```

### 3.2 HTTP状态码

| 状态码 | 说明 | 使用场景 |
|--------|------|----------|
| 200 | OK | 请求成功 |
| 201 | Created | 资源创建成功 |
| 204 | No Content | 删除成功 |
| 400 | Bad Request | 请求参数错误 |
| 401 | Unauthorized | 未认证 |
| 403 | Forbidden | 无权限 |
| 404 | Not Found | 资源不存在 |
| 409 | Conflict | 资源冲突（如重复） |
| 422 | Unprocessable Entity | 验证失败 |
| 500 | Internal Server Error | 服务器错误 |

---

## 4. API端点

### 4.1 学生管理 (Students)

#### 获取学生列表
```http
GET /api/v1/students
```

**查询参数**:
```
page        integer (optional) 页码，默认1
page_size   integer (optional) 每页数量，默认20
keyword     string (optional)   搜索关键词（姓名或学号）
grade       string (optional)   年级
major       string (optional)   专业
class_name  string (optional)   班级
sort_by     string (optional)   排序字段，默认created_at
order       string (optional)   排序方向，asc/desc，默认desc
```

**响应**:
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "items": [...],
    "pagination": {...}
  }
}
```

#### 获取单个学生
```http
GET /api/v1/students/{id}
```

**响应**:
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "id": "1234567890",
    "name": "张三",
    "student_id": "S20240001",
    "gender": "男",
    "birth_date": "2000-01-01",
    "grade": "2024",
    "class_name": "计算机1班",
    "major": "计算机科学与技术",
    "address": "北京市",
    "phone_number": "13800138000",
    "email": "zhangsan@example.com",
    "created_at": "2026-01-09T10:00:00Z",
    "updated_at": "2026-01-09T10:00:00Z"
  }
}
```

#### 创建学生
```http
POST /api/v1/students
Content-Type: application/json
```

**请求体**:
```json
{
  "name": "张三",
  "student_id": "S20240001",
  "gender": "男",
  "birth_date": "2000-01-01",
  "grade": "2024",
  "class_name": "计算机1班",
  "major": "计算机科学与技术",
  "address": "北京市",
  "phone_number": "13800138000",
  "email": "zhangsan@example.com",
  "father_name": "父亲",
  "mother_name": "母亲"
}
```

**响应**: `201 Created`

#### 更新学生
```http
PUT /api/v1/students/{id}
Content-Type: application/json
```

**请求体**: （同创建，全部字段）

**响应**: `200 OK`

#### 部分更新学生
```http
PATCH /api/v1/students/{id}
Content-Type: application/json
```

**请求体**:
```json
{
  "phone_number": "13900139000"
}
```

**响应**: `200 OK`

#### 删除学生
```http
DELETE /api/v1/students/{id}
```

**响应**: `204 No Content`

### 4.2 教师管理 (Teachers)

#### 获取教师列表
```http
GET /api/v1/teachers?page=1&page_size=20&keyword=王&department=计算机学院
```

**响应**: 同学生列表格式

#### 获取单个教师
```http
GET /api/v1/teachers/{id}
```

**响应**:
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "id": "1234567890",
    "name": "王教授",
    "teacher_id": "T001",
    "gender": "男",
    "birth_date": "1980-05-15",
    "department": "计算机学院",
    "position": "教授",
    "office": "A101",
    "phone_number": "13800138001",
    "email": "wang@example.com",
    "created_at": "2026-01-09T10:00:00Z",
    "updated_at": "2026-01-09T10:00:00Z"
  }
}
```

#### 创建教师
```http
POST /api/v1/teachers
```

**请求体**:
```json
{
  "name": "王教授",
  "teacher_id": "T001",
  "gender": "男",
  "birth_date": "1980-05-15",
  "department": "计算机学院",
  "position": "教授",
  "office": "A101",
  "phone_number": "13800138001",
  "email": "wang@example.com"
}
```

#### 更新/删除教师
```http
PUT    /api/v1/teachers/{id}
PATCH  /api/v1/teachers/{id}
DELETE /api/v1/teachers/{id}
```

### 4.3 课程管理 (Courses)

#### 获取课程列表
```http
GET /api/v1/courses?page=1&page_size=20&course_type=必修&semester=2024-春季
```

**响应**: 同学生列表格式

#### 获取单个课程
```http
GET /api/v1/courses/{id}
```

**响应**:
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "id": "1234567890",
    "course_id": "C001",
    "course_name": "数据结构",
    "teacher_id": "123",
    "teacher_name": "王教授",
    "credits": 3.0,
    "max_students": 100,
    "current_students": 45,
    "course_type": "必修",
    "semester": "2024-春季",
    "description": "计算机专业核心课程",
    "schedules": [
      {
        "day_of_week": "周一",
        "start_time": "08:00",
        "end_time": "09:40",
        "location": "A101",
        "week_range": "1-16周"
      }
    ],
    "created_at": "2026-01-09T10:00:00Z",
    "updated_at": "2026-01-09T10:00:00Z"
  }
}
```

#### 创建课程
```http
POST /api/v1/courses
```

**请求体**:
```json
{
  "course_id": "C001",
  "course_name": "数据结构",
  "teacher_id": "123",
  "credits": 3.0,
  "max_students": 100,
  "course_type": "必修",
  "semester": "2024-春季",
  "description": "计算机专业核心课程",
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
```

#### 更新/删除课程
```http
PUT    /api/v1/courses/{id}
PATCH  /api/v1/courses/{id}
DELETE /api/v1/courses/{id}
```

### 4.4 选课管理 (Enrollments)

#### 学生选课
```http
POST /api/v1/enrollments
```

**请求体**:
```json
{
  "student_id": "123",
  "course_id": "456"
}
```

#### 退课
```http
DELETE /api/v1/enrollments/{enrollment_id}
```

#### 获取学生的课程
```http
GET /api/v1/students/{student_id}/courses
```

#### 获取课程的学生列表
```http
GET /api/v1/courses/{course_id}/students
```

---

## 5. 错误处理

### 5.1 错误码规范

| 错误码 | 类型 | 说明 |
|--------|------|------|
| 1001 | ValidationError | 字段验证失败 |
| 1002 | DuplicateError | 资源重复（如学号已存在） |
| 1003 | NotFoundError | 资源不存在 |
| 1004 | UnauthorizedError | 未认证 |
| 1005 | ForbiddenError | 无权限 |
| 2001 | DatabaseError | 数据库错误 |
| 2002 | ExternalServiceError | 外部服务错误 |

### 5.2 错误响应示例

#### 验证错误
```json
{
  "code": 400,
  "message": "Validation Error",
  "error": {
    "type": "ValidationError",
    "details": [
      {"field": "name", "message": "姓名不能为空"},
      {"field": "student_id", "message": "学号格式错误"}
    ]
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

#### 资源不存在
```json
{
  "code": 404,
  "message": "Student Not Found",
  "error": {
    "type": "NotFoundError",
    "details": "ID为123的学生不存在"
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

#### 重复错误
```json
{
  "code": 409,
  "message": "Duplicate Student ID",
  "error": {
    "type": "DuplicateError",
    "field": "student_id",
    "value": "S20240001"
  },
  "timestamp": "2026-01-09T22:30:00Z"
}
```

---

## 6. 分页排序过滤

### 6.1 分页参数
```
page        页码，默认1，最小1
page_size   每页数量，默认20，范围1-100
```

### 6.2 排序参数
```
sort_by     排序字段
order       排序方向：asc/desc
```

示例：
```
GET /api/v1/students?sort_by=grade&order=asc
```

### 6.3 过滤参数

#### 精确匹配
```
GET /api/v1/students?grade=2024&major=计算机科学与技术
```

#### 模糊搜索
```
GET /api/v1/students?keyword=张
```

#### 范围查询
```
GET /api/v1/courses?credits_min=2&credits_max=4
```

#### 日期范围
```
GET /api/v1/students?created_after=2026-01-01&created_before=2026-12-31
```

---

## 7. 认证授权

### 7.1 JWT认证

#### 登录
```http
POST /api/v1/auth/login
```

**请求体**:
```json
{
  "username": "admin",
  "password": "password123"
}
```

**响应**:
```json
{
  "code": 200,
  "message": "Login successful",
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "expires_in": 3600,
    "user": {
      "id": "1",
      "username": "admin",
      "role": "admin"
    }
  }
}
```

#### 使用Token
```http
GET /api/v1/students
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

### 7.2 权限级别

| 角色 | 权限 |
|------|------|
| admin | 所有权限 |
| teacher | 查看所有、管理自己的课程 |
| student | 查看自己的信息、选课退课 |

---

## 8. API版本管理

### 8.1 版本策略
- 使用URL路径版本: `/api/v1/`, `/api/v2/`
- 主版本号变更表示破坏性更新
- 向后兼容的更新不改变版本号

### 8.2 废弃通知
```http
GET /api/v1/students
X-API-Deprecated: true
X-API-Sunset: 2026-12-31
X-API-Alternative: /api/v2/students
```

---

## 9. 限流

### 9.1 限流规则
- 匿名用户: 100请求/小时
- 已认证用户: 1000请求/小时
- 管理员: 无限制

### 9.2 限流响应
```http
HTTP/1.1 429 Too Many Requests
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 0
X-RateLimit-Reset: 1641792000

{
  "code": 429,
  "message": "Too Many Requests",
  "error": {
    "type": "RateLimitError",
    "retry_after": 3600
  }
}
```

---

## 10. 文档和测试

### 10.1 Swagger UI
访问地址: `http://localhost:20080/swagger`

### 10.2 Postman集合
提供完整的Postman测试集合

### 10.3 示例代码
提供多语言客户端SDK示例

---

## 11. 最佳实践

### 11.1 客户端实现
1. 使用HTTPS（生产环境）
2. 实现重试机制（幂等请求）
3. 缓存GET请求结果
4. 处理所有可能的错误状态码
5. 使用连接池

### 11.2 服务器实现
1. 输入验证和清理
2. SQL注入防护
3. XSS防护
4. CORS配置
5. 日志记录
6. 监控和告警

---

## 12. 变更日志

### v1.0.0 (2026-01-09)
- 初始版本
- 学生、教师、课程管理API
- 选课功能
- JWT认证
