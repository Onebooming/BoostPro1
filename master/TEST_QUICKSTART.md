# API 测试快速入门

## 文件说明

本项目包含两个API测试文件：

1. **test_api.sh** - 自动化测试脚本（推荐使用）
2. **API_TEST_GUIDE.md** - 完整的API测试文档

## 一分钟快速测试

### 1. 启动服务器

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### 2. 新开一个终端，运行测试

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master
./test_api.sh
```

## 测试脚本功能

`test_api.sh` 会自动测试以下内容：

### ✅ Student API (学生管理)
- ✅ GET 查询所有学生
- ✅ GET 按ID查询学生
- ✅ GET 按姓名查询学生
- ✅ POST 添加学生

### ✅ Teacher API (教师管理)
- ✅ GET 查询所有教师
- ✅ GET 按ID查询教师
- ✅ GET 按姓名查询教师
- ✅ POST 添加教师
- ✅ PUT 更新教师
- ✅ DELETE 删除教师

### ✅ Course API (课程管理)
- ✅ GET 查询所有课程
- ✅ GET 按ID查询课程
- ✅ GET 按名称查询课程
- ✅ POST 添加课程
- ✅ PUT 更新课程
- ✅ DELETE 删除课程

## 手动测试示例

### 测试 Student API

```bash
# 查询所有学生
curl -X GET http://127.0.0.1:20080/api/student

# 添加学生
curl -X POST http://127.0.0.1:20080/api/student \
  -H "Content-Type: application/json" \
  -d '{
    "name": "张三",
    "student_id": "S20240001",
    "gender": "男",
    "grade": "2024",
    "major": "计算机科学与技术"
  }'
```

### 测试 Teacher API

```bash
# 查询所有教师
curl -X GET http://127.0.0.1:20080/api/teacher

# 添加教师
curl -X POST http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{
    "name": "王教授",
    "teacher_id": "T20240001",
    "department": "计算机学院",
    "position": "教授"
  }'

# 更新教师
curl -X PUT http://127.0.0.1:20080/api/teacher \
  -H "Content-Type: application/json" \
  -d '{
    "id": "1",
    "name": "王教授",
    "teacher_id": "T20240001",
    "position": "正教授"
  }'
```

### 测试 Course API

```bash
# 查询所有课程
curl -X GET http://127.0.0.1:20080/api/course

# 添加课程
curl -X POST http://127.0.0.1:20080/api/course \
  -H "Content-Type: application/json" \
  -d '{
    "course_id": "C20240001",
    "course_name": "数据结构",
    "teacher_id": "T20240001",
    "credits": 3.0,
    "course_type": "必修"
  }'
```

## 查看详细文档

完整的API文档请查看：`API_TEST_GUIDE.md`

```bash
cat API_TEST_GUIDE.md
```

## 常见问题

### Q: 服务器连接失败
**A**: 确保服务器已启动
```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### Q: JSON格式错误
**A**: 使用python验证JSON格式
```bash
echo '{"name":"test"}' | python3 -m json.tool
```

### Q: 查询返回空数组
**A**: 数据库可能为空，先添加一些测试数据
