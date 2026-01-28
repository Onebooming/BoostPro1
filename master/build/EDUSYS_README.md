# 🎓 BoostPro 教务管理系统

## 系统简介

基于AdminLTE 4和Boost.Beast构建的现代化教务管理系统，支持学生管理、教师管理、课程管理和通知公告等功能模块。

## ✨ 功能特性

### 已实现功能

✅ **学生管理**
- 学生信息增删改查
- 学生列表展示
- 完整字段支持（姓名、学号、性别、出生日期、专业、班级等）

✅ **教师管理**
- 教师信息管理
- 教师列表展示

✅ **课程管理**
- 课程信息维护
- 课程列表展示

✅ **通知公告系统**
- 发布通知/新闻
- 优先级设置（低/中/高）
- 置顶功能
- 发布状态管理
- 浏览次数统计

✅ **仪表盘**
- 数据统计概览
- 最新通知展示
- 快捷操作入口

## 🚀 快速开始

### 1. 启动服务器

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

### 2. 访问系统

**教务管理系统主页：**
```
http://10.211.55.5:20080/edusys/index.html
```

**API测试页面：**
```
http://10.211.55.5:20080/api-demo.html
```

**AdminLTE框架：**
```
http://10.211.55.5:20080/static/adminlte/index.html
```

## 📁 项目结构

```
/home/parallels/chenglei_work/2025/BoostPro1/master/
├── server/src/
│   ├── entity/                    # 实体类
│   │   ├── student.cpp/hpp        # 学生实体
│   │   ├── teacher.cpp/hpp        # 教师实体
│   │   ├── course.cpp/hpp         # 课程实体
│   │   └── notification.hpp       # 通知实体
│   │
│   ├── dao/                       # 数据访问层
│   │   ├── student_dao.cpp/hpp    # 学生DAO
│   │   ├── teacher_dao.cpp/hpp    # 教师DAO
│   │   ├── course_dao.cpp/hpp     # 课程DAO
│   │   └── notification_dao.cpp/hpp  # 通知DAO
│   │
│   ├── controller/                # 控制器层
│   │   ├── student_controller.cpp/hpp
│   │   ├── teacher_controller.cpp/hpp
│   │   ├── course_controller.cpp/hpp
│   │   ├── notification_controller.hpp
│   │   └── notification_controller_wrapper.hpp
│   │
│   └── static/                    # 前端静态文件
│       ├── js/
│       │   └── api-client.js      # API客户端
│       │
│       └── edusys/                # 教务管理系统
│           └── index.html         # 系统主页
│
└── database/                      # 数据库脚本
    └── init.sql                   # 数据库初始化
```

## 🔌 API接口

### 通知API

**获取所有通知**
```
GET /api/notification?action=list_notifications
```

**获取已发布通知**
```
GET /api/notification?action=list_published_notifications
```

**根据ID查询**
```
POST /api/notification
Content-Type: application/json

{
  "action": "query_notification_by_id",
  "id": "123456789"
}
```

**创建通知**
```
POST /api/notification
Content-Type: application/json

{
  "action": "add_notification",
  "title": "系统维护通知",
  "content": "系统将于今晚22:00进行维护",
  "type": "notice",
  "priority": 3,
  "is_top": true,
  "publish": true
}
```

**更新通知**
```
POST /api/notification
Content-Type: application/json

{
  "action": "update_notification",
  "id": "123456789",
  "title": "更新后的标题",
  "content": "更新后的内容"
}
```

**删除通知**
```
POST /api/notification
Content-Type: application/json

{
  "action": "delete_notification",
  "id": "123456789"
}
```

**发布通知**
```
POST /api/notification
Content-Type: application/json

{
  "action": "publish_notification",
  "id": "123456789"
}
```

### 学生/教师/课程API

```
GET /api/student?action=list_students
GET /api/teacher?action=list_teachers
GET /api/course?action=list_courses
```

## 💻 前端开发

### 使用API客户端

```javascript
// 获取已发布通知
const response = await api.notification.getPublished();

// 创建新通知
await api.notification.create({
  title: '期中考试安排',
  content: '期中考试将于下周一开始...',
  type: 'notice',
  priority: 2,
  publisher: '教务处',
  publish: true
});

// 发布通知
await api.notification.publish('123456789');

// 删除通知
await api.notification.delete('123456789');
```

## 🗄️ 数据库表

### notifications表

```sql
CREATE TABLE notifications (
  id VARCHAR(64) PRIMARY KEY,
  title VARCHAR(200) NOT NULL,
  content TEXT,
  type VARCHAR(20) NOT NULL DEFAULT 'notice',
  publisher VARCHAR(100),
  publisher_id VARCHAR(64),
  priority INT DEFAULT 2,
  is_top BOOLEAN DEFAULT FALSE,
  is_published BOOLEAN DEFAULT FALSE,
  view_count INT DEFAULT 0,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  published_at DATETIME
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

## 🎨 界面展示

### 仪表盘
- 数据统计卡片（学生、教师、课程、通知数量）
- 最新通知列表
- 快捷操作按钮

### 管理模块
- 学生管理：表格展示、增删改查
- 教师管理：表格展示、信息维护
- 课程管理：表格展示、课程信息
- 通知管理：发布、编辑、删除、发布控制

## 🔧 技术栈

### 后端
- C++20
- Boost.Beast (HTTP服务器)
- MySQL (数据库)
- nlohmann/json (JSON处理)

### 前端
- AdminLTE 4 (UI框架)
- Bootstrap 5
- Vanilla JavaScript
- 自定义API客户端

## 📝 待完善功能

- [ ] 添加/编辑模态框实现
- [ ] 表单验证
- [ ] 分页功能
- [ ] 搜索和筛选
- [ ] 批量操作
- [ ] 导出功能
- [ ] 权限管理
- [ ] 登录认证

## 🐛 故障排查

### 1. 通知表不存在
启动服务器时会自动创建notifications表，如果失败请手动执行：

```bash
mysql -u root -p
use boostpro1;
source database/init.sql;
```

### 2. 页面无法访问
- 确认服务器正在运行：`ps aux | grep boost_server`
- 检查端口占用：`netstat -tuln | grep 20080`
- 查看服务器日志

### 3. API调用失败
打开浏览器开发者工具查看Network和Console面板，检查：
- 请求URL是否正确
- 响应状态码
- 错误信息

## 📞 联系方式

如有问题，请查看相关文档或提交issue。

---

**版本**: v1.0.0
**更新时间**: 2025-01-18
**开发者**: BoostPro Team
