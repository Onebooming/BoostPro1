# 课程管理系统使用指南

## 概述
教务系统的课程管理模块已完善，支持完整的CRUD（增删改查）功能，并能显示课程的时间段和教室信息。

## 访问地址
- **前端页面**: http://10.211.55.5:20080/edusys/index.html
- **API端点**: http://10.211.55.5:20080/api/course

## 功能特性

### 1. 课程列表显示
- ✅ 显示所有课程信息
- ✅ 展示课程名称、课程号、授课教师、学分、类型、学期
- ✅ **显示上课时间地点**：包含星期、时间、教室、周次范围
- ✅ 支持多时间段课程（例如：周一上午和周三下午）

### 2. 添加课程
- ✅ 完整的课程信息录入
- ✅ **动态添加多个上课时间段**
- ✅ 每个时间段包含：
  - 星期（周一至周日）
  - 开始时间（如：08:00）
  - 结束时间（如：09:40）
  - 教室/地点（如：A楼101）
  - 周次范围（如：1-16周）

### 3. 编辑课程
- ✅ 修改课程基本信息
- ✅ 添加/删除/修改上课时间段
- ✅ 保存后自动刷新列表

### 4. 查看课程详情
- ✅ 显示完整课程信息
- ✅ 列出所有上课时间安排

### 5. 删除课程
- ✅ 删除前确认提示
- ✅ 删除后自动更新统计数据

## 使用方法

### 启动服务器
```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### 访问系统
1. 打开浏览器
2. 访问: http://10.211.55.5:20080/edusys/index.html
3. 点击左侧菜单的"课程管理"图标

### 添加课程示例
1. 点击"添加课程"按钮
2. 填写课程信息：
   - 课程名称：数据结构
   - 课程号：CS101
   - 教师ID：T001
   - 学分：4
   - 课程类型：必修
   - 学期：2024-2025-1
   - 最大人数：80
3. 点击"添加时间段"按钮，填写上课时间：
   - 星期：周一
   - 开始时间：08:00
   - 结束时间：09:40
   - 地点：A楼301
   - 周次：1-16周
4. 可以继续添加更多时间段（如周三10:00-11:40）
5. 点击"提交"按钮

## API 接口说明

### 查询课程列表
```bash
GET /api/course?action=list_courses
```

### 查询单个课程
```bash
GET /api/course?action=query_course_by_id&id=<课程ID>
```

### 添加课程
```bash
POST /api/course
Content-Type: application/json

{
  "action": "add_course",
  "course_id": "CS101",
  "course_name": "数据结构",
  "teacher_id": "T001",
  "credits": 4,
  "course_type": "必修",
  "semester": "2024-2025-1",
  "max_students": 80,
  "description": "计算机专业核心课程",
  "schedules": [
    {
      "day_of_week": "周一",
      "start_time": "08:00",
      "end_time": "09:40",
      "location": "A楼301",
      "week_range": "1-16周"
    }
  ]
}
```

### 更新课程
```bash
POST /api/course
Content-Type: application/json

{
  "action": "update_course",
  "id": "<课程ID>",
  "course_name": "数据结构（修订版）",
  ...
}
```

### 删除课程
```bash
POST /api/course
Content-Type: application/json

{
  "action": "delete_course",
  "id": "<课程ID>"
}
```

## 数据结构

### 课程对象 (CourseDetailed)
```json
{
  "id": "雪花ID",
  "course_id": "课程编号",
  "course_name": "课程名称",
  "teacher_id": "教师ID",
  "teacher_name": "教师姓名",
  "credits": 3.0,
  "max_students": 60,
  "current_students": 45,
  "course_type": "必修/选修/专业选修",
  "semester": "2024-2025-1",
  "description": "课程描述",
  "created_at": "创建时间",
  "updated_at": "更新时间",
  "schedules": [
    {
      "day_of_week": "周一",
      "start_time": "08:00",
      "end_time": "09:40",
      "location": "A楼301",
      "week_range": "1-16周"
    }
  ]
}
```

## 前端实现要点

### 1. 课程表格显示
- 使用`generateCourseTable()`函数生成表格
- 时间地点信息格式化显示，使用`<hr>`分隔多个时间段

### 2. 模态框管理
- `showAddCourseModal()`: 显示添加课程模态框
- `editCourse(id)`: 编辑指定课程
- `saveCourse()`: 保存课程（新增或更新）

### 3. 时间段管理
- `addCourseSchedule()`: 动态添加时间段输入框
- `removeSchedule(index)`: 删除指定时间段

## 技术栈

### 后端
- C++ with Boost.Beast
- MySQL数据库
- nlohmann/json (JSON处理)

### 前端
- AdminLTE 3.2
- Bootstrap 5.3
- Bootstrap Icons
- 原生JavaScript (ES6+)

## 测试脚本

测试脚本位于：`/home/parallels/chenglei_work/2025/BoostPro1/master/build/test_course_management.sh`

运行测试：
```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./test_course_management.sh
```

## 常见问题

### Q: 为什么看不到课程列表？
A: 确保服务器已启动，且数据库中有课程数据。可以点击"刷新"按钮重新加载。

### Q: 添加课程后没有显示？
A: 检查浏览器控制台是否有错误信息，确认网络请求成功。

### Q: 如何删除课程？
A: 点击课程列表中的"删除"按钮，确认后即可删除。

### Q: 可以添加多个上课时间段吗？
A: 可以！在添加/编辑课程时，点击"添加时间段"按钮即可添加多个时间安排。

## 系统限制

- 课程名称为必填项
- 教师ID默认为T001（可修改）
- 时间段可以不添加（课程无固定时间）
- 删除课程不可恢复

## 后续改进建议

1. 添加教师选择下拉框（从教师列表加载）
2. 添加课程时间冲突检测
3. 支持批量导入课程
4. 添加课程选课人数统计图表
5. 支持课程搜索和筛选功能

## 版本信息

- **开发时间**: 2025年1月
- **版本**: v1.0
- **状态**: ✅ 完成并测试通过

---

**祝使用愉快！**
