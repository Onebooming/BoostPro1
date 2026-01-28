# 🎓 BoostPro 完整教务管理系统

## 📊 系统概述

基于Boost.Beast + AdminLTE 4构建的完整教务管理系统，包含以下核心模块。

## ✅ 已完成模块

### 后端模块 (C++20 + Boost.Beast + MySQL)

| 模块 | Entity | DAO | Controller | API路由 | 状态 |
|------|--------|-----|------------|---------|------|
| 学生管理 | ✅ | ✅ | ✅ | ✅ | ✅ 完整实现 |
| 教师管理 | ✅ | ✅ | ✅ | ✅ | ✅ 完整实现 |
| 课程管理 | ✅ | ✅ | ✅ | ✅ | ✅ 完整实现 |
| 教室管理 | ✅ | ✅ | ✅ | ✅ | ✅ 完整实现 |
| 通知管理 | ✅ | ✅ | ✅ | ⚠️ | ⚠️ 暂禁用 |
| 学校新闻 | ✅ | ⏳ | ⏳ | ⏳ | 🚧 待完善 |
| 知识博客 | ✅ | ⏳ | ⏳ | ⏳ | 🚧 待完善 |

### 前端模块 (AdminLTE 4 + Bootstrap 5 + Vanilla JS)

| 模块 | 列表展示 | 添加功能 | 编辑功能 | 删除功能 | 详情查看 | 状态 |
|------|----------|----------|----------|----------|----------|------|
| 仪表盘 | ✅ | N/A | N/A | N/A | N/A | ✅ 完整 |
| 学生管理 | ✅ | ✅ | ⏳ | ⏳ | ⏳ | ✅ 可用 |
| 教师管理 | ✅ | ⏳ | ⏳ | ⏳ | ⏳ | 🚧 待完善 |
| 课程管理 | ✅ | ⏳ | ⏳ | ⏳ | ⏳ | 🚧 待完善 |
| 教室管理 | ⏳ | ⏳ | ⏳ | ⏳ | ⏳ | 🚧 待添加 |

## 🚀 快速开始

### 1. 编译并启动服务器

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
make clean && make
./boost_server
```

### 2. 访问系统

- **教务管理系统**: http://10.211.55.5:20080/edusys/index.html
- **API测试页面**: http://10.211.55.5:20080/static/test.html

## 📁 项目结构

```
/home/parallels/chenglei_work/2025/BoostPro1/master/
├── server/src/
│   ├── entity/                      # 实体类
│   │   ├── student.cpp/hpp         # ✅ 学生实体
│   │   ├── teacher.cpp/hpp         # ✅ 教师实体
│   │   ├── course.cpp/hpp          # ✅ 课程实体
│   │   ├── classroom.hpp           # ✅ 教室实体
│   │   ├── notification.hpp        # ✅ 通知实体
│   │   ├── news.hpp                # ✅ 新闻实体
│   │   └── blog.hpp                # ✅ 博客实体
│   │
│   ├── dao/                        # 数据访问层
│   │   ├── student_dao.cpp/hpp     # ✅ 学生DAO
│   │   ├── teacher_dao.cpp/hpp     # ✅ 教师DAO
│   │   ├── course_dao.cpp/hpp      # ✅ 课程DAO
│   │   ├── classroom_dao.cpp/hpp   # ✅ 教室DAO
│   │   └── notification_dao.cpp/hpp # ✅ 通知DAO
│   │
│   ├── controller/                 # 控制器层
│   │   ├── student_controller.hpp  # ✅ 学生控制器
│   │   ├── teacher_controller.hpp  # ✅ 教师控制器
│   │   ├── course_controller.hpp   # ✅ 课程控制器
│   │   ├── classroom_controller.hpp # ✅ 教室控制器
│   │   └── notification_controller_wrapper.hpp # ✅ 通知控制器
│   │
│   └── static/                     # 前端静态文件
│       ├── js/
│       │   └── api-client.js       # ✅ API客户端（已支持教室）
│       │
│       └── edusys/                 # 教务管理系统
│           └── index.html          # ✅ 系统主页
│
├── database/                       # 数据库脚本
└── build/                          # 编译输出目录
```

## 🔌 API接口文档

### 教室管理 API

**获取所有教室**
```
GET /api/classroom?action=list_classrooms
```

**根据ID查询**
```
GET /api/classroom?action=query_by_id&id={id}
```

**添加教室**
```
POST /api/classroom
Content-Type: application/json

{
  "action": "add_classroom",
  "room_number": "A101",
  "building": "A栋",
  "floor": 1,
  "capacity": 60,
  "room_type": "多媒体教室",
  "has_projector": true,
  "has_computer": true,
  "has_audio": true,
  "status": "可用"
}
```

**更新教室**
```
POST /api/classroom
Content-Type: application/json

{
  "action": "update_classroom",
  "id": "{id}",
  "room_number": "A102",
  "capacity": 80
}
```

**删除教室**
```
POST /api/classroom
Content-Type: application/json

{
  "action": "delete_classroom",
  "id": "{id}"
}
```

**根据楼栋查询**
```
GET /api/classroom?action=list_by_building&building=A栋
```

**根据类型查询**
```
GET /api/classroom?action=list_by_type&type=多媒体教室
```

**根据状态查询**
```
GET /api/classroom?action=list_by_status&status=可用
```

## 💻 前端使用示例

### JavaScript API调用

```javascript
// 获取所有教室
const classrooms = await api.classroom.getList();

// 添加新教室
await api.classroom.create({
  room_number: 'A101',
  building: 'A栋',
  floor: 1,
  capacity: 60,
  room_type: '多媒体教室',
  has_projector: true,
  has_computer: true,
  has_audio: true
});

// 更新教室
await api.classroom.update('123456', { capacity: 80 });

// 删除教室
await api.classroom.delete('123456');

// 根据条件查询
const availableRooms = await api.classroom.getByStatus('可用');
const multiMediaRooms = await api.classroom.getByType('多媒体教室');
```

## 🗄️ 数据库表

### classrooms表

```sql
CREATE TABLE classrooms (
  id VARCHAR(64) PRIMARY KEY,
  room_number VARCHAR(50) NOT NULL UNIQUE,
  building VARCHAR(50) NOT NULL,
  floor INT DEFAULT 1,
  capacity INT DEFAULT 40,
  room_type VARCHAR(50) DEFAULT '普通教室',
  has_projector BOOLEAN DEFAULT FALSE,
  has_computer BOOLEAN DEFAULT FALSE,
  has_audio BOOLEAN DEFAULT FALSE,
  status VARCHAR(20) DEFAULT '可用',
  remark TEXT,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  INDEX idx_building (building),
  INDEX idx_type (room_type),
  INDEX idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

## 🎨 功能特性

### 已实现功能

✅ **学生管理**
- 学生信息增删改查
- 学生列表展示
- 完整字段支持（姓名、学号、性别、出生日期、专业、班级等）
- 添加学生模态框

✅ **教师管理**
- 教师信息查询
- 教师列表展示

✅ **课程管理**
- 课程信息查询
- 课程列表展示

✅ **教室管理**
- 教室信息增删改查
- 按楼栋查询
- 按类型查询（普通教室、多媒体教室、实验室等）
- 按状态查询（可用、维修中、占用）
- 设施信息（投影仪、计算机、音响）

✅ **仪表盘**
- 数据统计概览
- 最新通知展示
- 快捷操作入口

### 待完善功能

- [ ] 教师添加/编辑/删除功能
- [ ] 课程添加/编辑/删除功能
- [ ] 教室管理前端界面
- [ ] 学校新闻管理（后端实体已创建）
- [ ] 知识博客管理（后端实体已创建）
- [ ] 分页功能
- [ ] 搜索和筛选
- [ ] 批量操作
- [ ] 导出功能
- [ ] 权限管理
- [ ] 登录认证

## 🔧 技术栈

### 后端
- C++20
- Boost.Beast (HTTP服务器)
- MySQL 8.0 (数据库)
- nlohmann/json (JSON处理)
- Snowflake ID生成

### 前端
- AdminLTE 4 (UI框架)
- Bootstrap 5.3.2
- Bootstrap Icons 1.13.1
- Vanilla JavaScript (ES6+)
- 自定义API客户端

## 📊 当前数据统计

- **学生数量**: 6名
- **教师数量**: 已有数据库表
- **课程数量**: 已有数据库表
- **教室数量**: 待添加数据

## 🐛 故障排查

### 1. 编译错误

**问题**: 未定义的引用

**解决**:
```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
cmake ..
make clean && make
```

### 2. 页面加载慢或资源加载失败

**原因**: CDN资源访问问题

**解决**:
- 已配置使用国内BootCDN镜像
- 检查网络连接
- 清除浏览器缓存（Ctrl + F5）

### 3. 模态框无法弹出

**原因**: Bootstrap JS加载顺序问题

**解决**: 已修复 - Bootstrap JS现在在自定义JS之前加载

## 📝 下一步计划

### 短期目标 (1-2天)

1. ✅ 完成教室管理后端
2. ⏳ 添加教室管理前端界面
3. ⏳ 完善教师管理的添加/编辑/删除功能
4. ⏳ 完善课程管理的添加/编辑/删除功能

### 中期目标 (3-5天)

1. 创建学校新闻完整模块（DAO + Controller + 前端）
2. 创建知识博客完整模块（DAO + Controller + 前端）
3. 添加分页功能
4. 添加搜索和筛选

### 长期目标 (1-2周)

1. 实现用户认证和权限管理
2. 添加数据导出功能（Excel）
3. 优化性能和用户体验
4. 添加系统日志记录

## 🎯 快速添加新模块指南

### 以"学校新闻"为例：

#### 1. 创建DAO层 (5分钟)

参考 `notification_dao.cpp`，创建 `news_dao.cpp`：

```cpp
// 主要方法：
- createTable()  // 创建表
- insertNews()   // 添加新闻
- updateNews()   // 更新新闻
- deleteNews()   // 删除新闻
- selectNewsById()  // 根据ID查询
- selectAllNews()   // 获取所有新闻
- selectPublishedNews() // 获取已发布新闻
```

#### 2. 创建Controller (10分钟)

参考 `notification_controller.hpp`，创建 `news_controller.hpp`

#### 3. 注册到API路由器 (1分钟)

在 `api_url_handler.cpp` 中添加：
```cpp
#include "../controller/news_controller.hpp"
// ...
chenglei::ApiRouterMgr::getInstance().registerController(
    "news",
    std::make_unique<chenglei::NewsController>());
```

#### 4. 添加前端API支持 (5分钟)

在 `api-client.js` 中添加 NewsApi 类

#### 5. 创建前端界面 (30分钟)

在 `index.html` 中添加新闻管理section和JavaScript函数

**总计**: 约50分钟即可添加一个完整的新模块！

## 📞 技术支持

- **项目路径**: `/home/parallels/chenglei_work/2025/BoostPro1/master/`
- **构建目录**: `/home/parallels/chenglei_work/2025/BoostPro1/master/build/`
- **服务端口**: 20080
- **数据库**: MySQL (boostpro1)

---

**版本**: v2.0.0
**更新时间**: 2025-01-22
**开发者**: BoostPro Team
**License**: MIT
