# 外部设备访问服务器配置指南

## 概述

本指南说明如何配置系统，使外部设备能够访问本机的 20080 端口。

## 当前配置状态

### ✅ 服务器配置（已正确）
- **绑定地址**: `0.0.0.0` (所有网络接口)
- **监听端口**: `20080`
- **配置文件**: `public/basedata.hpp`

```cpp
static const std::string HTTP_SERVER_ADDR = "0.0.0.0";
static const unsigned short HTTP_PORT = 20080;
```

### 📋 网络信息
- **本机IP地址**: `10.211.55.5`
- **子网掩码**: `/24 (255.255.255.0)`
- **广播地址**: `10.211.55.255`

---

## 方法一：配置防火墙规则（推荐）

### Ubuntu/Debian 使用 UFW

#### 1. 检查防火墙状态
```bash
sudo ufw status
```

#### 2. 如果防火墙未激活，先启用
```bash
# 允许SSH连接（重要！防止锁死）
sudo ufw allow 22/tcp

# 允许HTTP端口
sudo ufw allow 20080/tcp

# 启用防火墙
sudo ufw enable
```

#### 3. 如果防火墙已激活，添加规则
```bash
# 允许20080端口
sudo ufw allow 20080/tcp

# 查看规则
sudo ufw status numbered
```

#### 4. 删除规则（如果需要）
```bash
# 查看编号规则
sudo ufw status numbered

# 删除指定编号的规则
sudo ufw delete [规则编号]
```

### CentOS/RHEL 使用 firewalld

#### 1. 检查防火墙状态
```bash
sudo firewall-cmd --state
```

#### 2. 开放端口
```bash
# 添加端口
sudo firewall-cmd --permanent --add-port=20080/tcp

# 重载防火墙
sudo firewall-cmd --reload

# 查看开放的端口
sudo firewall-cmd --list-ports
```

#### 3. 关闭端口（如果需要）
```bash
sudo firewall-cmd --permanent --remove-port=20080/tcp
sudo firewall-cmd --reload
```

### 使用 iptables（通用方法）

#### 1. 添加规则
```bash
# 允许TCP端口20080
sudo iptables -A INPUT -p tcp --dport 20080 -j ACCEPT

# 保存规则（Ubuntu/Debian）
sudo iptables-save | sudo tee /etc/iptables/rules.v4

# 保存规则（CentOS/RHEL）
sudo service iptables save
```

#### 2. 查看规则
```bash
sudo iptables -L -n -v | grep 20080
```

#### 3. 删除规则
```bash
# 查看规则编号
sudo iptables -L INPUT --line-numbers -n

# 删除指定编号的规则
sudo iptables -D INPUT [规则编号]
```

---

## 方法二：临时关闭防火墙（仅用于测试）

⚠️ **警告**: 这会降低系统安全性，仅建议在测试环境中使用！

### Ubuntu/Debian
```bash
sudo ufw disable
```

### CentOS/RHEL
```bash
sudo systemctl stop firewalld
```

---

## 方法三：使用 SSH 端口转发（无需配置防火墙）

如果你有SSH访问权限，可以使用端口转发来访问服务器。

### 从外部设备 SSH 隧道

```bash
# 在外部设备上执行
ssh -L 20080:localhost:20080 user@10.211.55.5
```

然后在外部设备上访问 `http://localhost:20080`

---

## 验证配置

### 1. 检查端口监听状态
```bash
# 方法1: 使用 netstat
sudo netstat -tuln | grep 20080

# 方法2: 使用 ss
sudo ss -tuln | grep 20080

# 方法3: 使用 lsof
sudo lsof -i :20080
```

**预期输出**:
```
tcp  0  0  0.0.0.0:20080  0.0.0.0:*  LISTEN
```

### 2. 本机测试
```bash
# 测试本地访问
curl http://127.0.0.1:20080/api/student

# 测试IP访问
curl http://10.211.55.5:20080/api/student
```

### 3. 外部设备测试

在同一局域网的其他设备上：
```bash
# 假设服务器IP为 10.211.55.5
curl http://10.211.55.5:20080/api/student
```

或在浏览器中访问：
```
http://10.211.55.5:20080/api/student
http://10.211.55.5:20080/static/adminlte/index.html
```

---

## 故障排查

### 问题1: 无法访问

**检查清单**:
1. ✅ 服务器是否运行？
   ```bash
   ps aux | grep boost_server
   ```

2. ✅ 端口是否监听？
   ```bash
   sudo netstat -tuln | grep 20080
   ```

3. ✅ 防火墙是否开放？
   ```bash
   sudo ufw status  # Ubuntu
   sudo firewall-cmd --list-ports  # CentOS
   ```

4. ✅ 网络是否连通？
   ```bash
   ping 10.211.55.5
   ```

### 问题2: Connection refused

**可能原因**:
- 服务器未启动
- 端口被其他程序占用
- 防火墙阻止

**解决方案**:
```bash
# 检查服务器
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server

# 检查端口占用
sudo lsof -i :20080
```

### 问题3: Connection timed out

**可能原因**:
- 防火墙阻止
- 网络不通
- 服务器绑定到127.0.0.1

**解决方案**:
```bash
# 检查绑定地址
grep HTTP_SERVER_ADDR /home/parallels/chenglei_work/2025/BoostPro1/master/public/basedata.hpp

# 应该是: static const std::string HTTP_SERVER_ADDR = "0.0.0.0";
```

---

## 安全建议

### 1. 限制访问来源IP
如果只允许特定IP访问，可以这样配置：

**UFW**:
```bash
# 只允许特定IP访问
sudo ufw allow from 192.168.1.100 to any port 20080

# 只允许特定网段访问
sudo ufw allow from 10.211.55.0/24 to any port 20080
```

**firewalld**:
```bash
sudo firewall-cmd --permanent --add-rich-rule='rule family="ipv4" source address="10.211.55.0/24" port protocol="tcp" port="20080" accept'
sudo firewall-cmd --reload
```

### 2. 使用反向代理
在生产环境，建议使用 Nginx 作为反向代理：

```nginx
server {
    listen 80;
    server_name your-domain.com;

    location / {
        proxy_pass http://127.0.0.1:20080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### 3. 启用 HTTPS
使用 Let's Encrypt 证书：
```bash
sudo certbot --nginx -d your-domain.com
```

---

## 访问地址总结

配置完成后，外部设备可以通过以下地址访问：

### API 端点
- **Student API**: `http://10.211.55.5:20080/api/student`
- **Teacher API**: `http://10.211.55.5:20080/api/teacher`
- **Course API**: `http://10.211.55.5:20080/api/course`

### 前端页面
- **AdminLTE**: `http://10.211.55.5:20080/static/adminlte/index.html`
- **首页**: `http://10.211.55.5:20080/index.html`

---

## 快速配置命令

### Ubuntu/Debian 一键配置
```bash
# 允许端口
sudo ufw allow 20080/tcp

# 如果防火墙未启用
sudo ufw enable

# 验证
sudo ufw status
```

### CentOS/RHEL 一键配置
```bash
# 允许端口
sudo firewall-cmd --permanent --add-port=20080/tcp

# 重载
sudo firewall-cmd --reload

# 验证
sudo firewall-cmd --list-ports
```

---

## 注意事项

1. **确保服务器正在运行**
   ```bash
   cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
   ./boost_server
   ```

2. **检查IP地址变化**
   如果使用DHCP，IP地址可能会变化。建议配置静态IP。

3. **网络隔离**
   如果在不同网络（如跨路由器），需要配置路由转发或端口映射。

4. **云服务器**
   如果是云服务器（阿里云、腾讯云等），还需要在云控制台配置安全组规则。

---

## 总结

要让外部设备访问20080端口：

1. ✅ **服务器已配置** - 绑定到 0.0.0.0:20080
2. 🔧 **需要配置防火墙** - 开放20080端口
3. 🌐 **使用正确IP** - 10.211.55.5
4. 🧪 **测试验证** - curl 或浏览器访问

**快速开始**:
```bash
# 1. 确保服务器运行
cd /home/parallels/chenglei_work/2025/BoostPro1/master/build
./boost_server

# 2. 开放防火墙（Ubuntu）
sudo ufw allow 20080/tcp

# 3. 在外部设备测试
curl http://10.211.55.5:20080/api/student
```
