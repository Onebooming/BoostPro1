# API演示页面访问指南

## 问题修复

已修复静态文件路由问题！现在可以访问 `/api-demo.html` 了。

## 修复内容

### 问题描述
访问 `http://10.211.55.5:20080/api-demo.html` 返回 "Invalid Url"

### 原因
静态文件处理器只处理 `/static/*` 和 `/` 路径，不支持根路径下的 `.html` 文件

### 修复方案
修改了 `static_url_handler.cpp`，现在支持：
- ✅ `/` → `static/index.html`
- ✅ `/api-demo.html` → `static/api-demo.html`
- ✅ `/xxx.html` → `static/xxx.html`
- ✅ `/static/*` → `static/*`

## 访问方式

### 方式1：直接访问（推荐）
```
http://10.211.55.5:20080/api-demo.html
```

### 方式2：通过static路径
```
http://10.211.55.5:20080/static/api-demo.html
```

### 方式3：本地访问
```
http://localhost:20080/api-demo.html
http://127.0.0.1:20080/api-demo.html
```

## 启动服务器

```bash
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server
```

## 验证访问

服务器启动后，在浏览器中访问：

```
http://10.211.55.5:20080/api-demo.html
```

你应该能看到：
- ✅ BoostPro API 测试页面
- ✅ 三个标签页：学生管理、教师管理、课程管理
- ✅ 完整的CRUD功能界面
- ✅ 实时API响应显示

## 测试功能

### 1. 学生管理
- 点击"刷新列表"查看所有学生
- 点击"添加学生"创建新学生
- 点击"查看"按钮查看学生详情
- 点击"删除"按钮删除学生

### 2. 教师管理
- 切换到"教师管理"标签
- 测试教师增删改查功能

### 3. 课程管理
- 切换到"课程管理"标签
- 测试课程增删改查功能

## API调用说明

### 前端JavaScript API

在页面中，可以使用以下API：

```javascript
// 学生API
await api.student.getList({ page: 1, page_size: 20 });
await api.student.getById('123');
await api.student.create({ name: '张三', student_id: 'S001' });
await api.student.update('123', { name: '李四' });
await api.student.delete('123');

// 教师API
await api.teacher.getList();
await api.teacher.getById('123');
await api.teacher.create(data);

// 课程API
await api.course.getList();
await api.course.getById('123');
await api.course.create(data);
```

### 响应格式

所有API返回统一格式：

```json
{
  "code": 200,
  "message": "success",
  "data": {...},
  "timestamp": "2026-01-09T22:30:00Z"
}
```

## 故障排查

### 如果仍然无法访问

1. **确认服务器正在运行**
   ```bash
   ps aux | grep boost_server
   ```

2. **确认文件存在**
   ```bash
   ls -lh /home/parallels/chenglei_work/2025/BoostPro1/master/build/static/api-demo.html
   ```

3. **查看服务器日志**
   服务器会输出每个请求的信息，注意查看错误信息

4. **测试静态文件路由**
   ```bash
   curl http://localhost:20080/api-demo.html
   ```

### 防火墙问题

如果从外部设备无法访问：

```bash
# 开放防火墙端口
sudo ufw allow 20080/tcp
```

## 相关文档

- **API_DESIGN.md** - RESTful API设计规范
- **RESTFUL_API_GUIDE.md** - 完整集成指南
- **README_RESTFUL.md** - 快速参考

## 总结

✅ 已修复静态文件访问问题
✅ 支持根路径下的 .html 文件访问
✅ 编译成功，可以测试

现在可以访问 http://10.211.55.5:20080/api-demo.html 测试完整的API功能了！🎉
