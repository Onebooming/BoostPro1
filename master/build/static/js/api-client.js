/**
 * BoostPro API 客户端
 * 提供统一的RESTful API调用接口
 */

class BoostProApiClient {
    constructor(baseURL = 'http://localhost:20080/api/v1') {
        this.baseURL = baseURL;
        this.token = localStorage.getItem('auth_token') || null;
    }

    /**
     * 设置认证Token
     */
    setToken(token) {
        this.token = token;
        if (token) {
            localStorage.setItem('auth_token', token);
        } else {
            localStorage.removeItem('auth_token');
        }
    }

    /**
     * 获取请求头
     */
    getHeaders() {
        const headers = {
            'Content-Type': 'application/json'
        };

        if (this.token) {
            headers['Authorization'] = `Bearer ${this.token}`;
        }

        return headers;
    }

    /**
     * 通用请求方法
     */
    async request(url, options = {}) {
        const config = {
            ...options,
            headers: {
                ...this.getHeaders(),
                ...options.headers
            }
        };

        try {
            const response = await fetch(`${this.baseURL}${url}`, config);
            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.message || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error('API Request Error:', error);
            throw error;
        }
    }

    /**
     * GET请求
     */
    async get(url, params = {}) {
        const queryString = new URLSearchParams(params).toString();
        const fullUrl = queryString ? `${url}?${queryString}` : url;
        return this.request(fullUrl, { method: 'GET' });
    }

    /**
     * POST请求
     */
    async post(url, data = {}) {
        return this.request(url, {
            method: 'POST',
            body: JSON.stringify(data)
        });
    }

    /**
     * PUT请求
     */
    async put(url, data = {}) {
        return this.request(url, {
            method: 'PUT',
            body: JSON.stringify(data)
        });
    }

    /**
     * PATCH请求
     */
    async patch(url, data = {}) {
        return this.request(url, {
            method: 'PATCH',
            body: JSON.stringify(data)
        });
    }

    /**
     * DELETE请求
     */
    async delete(url) {
        return this.request(url, { method: 'DELETE' });
    }
}

/**
 * 学生API
 */
class StudentApi {
    constructor(client) {
        this.client = client;
        this.resource = '/student';
    }

    /**
     * 获取学生列表
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_students', ...params });
    }

    /**
     * 获取单个学生
     * @param {string} id - 学生ID
     */
    async getById(id) {
        return this.client.get(this.resource, { action: 'query_student_by_id', id: id });
    }

    /**
     * 创建学生
     * @param {Object} data - 学生数据
     */
    async create(data) {
        return this.client.post(this.resource, { action: 'add_student', ...data });
    }

    /**
     * 更新学生
     * @param {string} id - 学生ID
     * @param {Object} data - 学生数据
     */
    async update(id, data) {
        return this.client.post(this.resource, { action: 'update_student', id: id, ...data });
    }

    /**
     * 删除学生
     * @param {string} id - 学生ID
     */
    async delete(id) {
        return this.client.post(this.resource, { action: 'delete_student', id: id });
    }
}

/**
 * 教师API
 */
class TeacherApi {
    constructor(client) {
        this.client = client;
        this.resource = '/teacher';
    }

    /**
     * 获取教师列表
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_teachers', ...params });
    }

    /**
     * 获取单个教师
     * @param {string} id - 教师ID
     */
    async getById(id) {
        return this.client.get(this.resource, { action: 'query_teacher_by_id', id: id });
    }

    /**
     * 创建教师
     * @param {Object} data - 教师数据
     */
    async create(data) {
        return this.client.post(this.resource, { action: 'add_teacher', ...data });
    }

    /**
     * 更新教师
     * @param {string} id - 教师ID
     * @param {Object} data - 教师数据
     */
    async update(id, data) {
        return this.client.post(this.resource, { action: 'update_teacher', id: id, ...data });
    }

    /**
     * 删除教师
     * @param {string} id - 教师ID
     */
    async delete(id) {
        return this.client.post(this.resource, { action: 'delete_teacher', id: id });
    }
}

/**
 * 课程API
 */
class CourseApi {
    constructor(client) {
        this.client = client;
        this.resource = '/course';
    }

    /**
     * 获取课程列表
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_courses', ...params });
    }

    /**
     * 获取单个课程
     * @param {string} id - 课程ID
     */
    async getById(id) {
        return this.client.get(this.resource, { action: 'query_course_by_id', id: id });
    }

    /**
     * 创建课程
     * @param {Object} data - 课程数据
     */
    async create(data) {
        return this.client.post(this.resource, { action: 'add_course', ...data });
    }

    /**
     * 更新课程
     * @param {string} id - 课程ID
     * @param {Object} data - 课程数据
     */
    async update(id, data) {
        return this.client.post(this.resource, { action: 'update_course', id: id, ...data });
    }

    /**
     * 删除课程
     * @param {string} id - 课程ID
     */
    async delete(id) {
        return this.client.post(this.resource, { action: 'delete_course', id: id });
    }
}

/**
 * 教室API
 */
class ClassroomApi {
    constructor(client) {
        this.client = client;
        this.resource = '/classroom';
    }

    /**
     * 获取教室列表
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_classrooms', ...params });
    }

    /**
     * 根据ID获取教室
     * @param {string} id - 教室ID
     */
    async getById(id) {
        return this.client.get(this.resource, { action: 'query_by_id', id: id });
    }

    /**
     * 创建教室
     * @param {Object} data - 教室数据
     */
    async create(data) {
        return this.client.post(this.resource, { action: 'add_classroom', ...data });
    }

    /**
     * 更新教室
     * @param {string} id - 教室ID
     * @param {Object} data - 教室数据
     */
    async update(id, data) {
        return this.client.post(this.resource, { action: 'update_classroom', id: id, ...data });
    }

    /**
     * 删除教室
     * @param {string} id - 教室ID
     */
    async delete(id) {
        return this.client.post(this.resource, { action: 'delete_classroom', id: id });
    }

    /**
     * 根据楼栋查询教室
     * @param {string} building - 楼栋
     */
    async getByBuilding(building) {
        return this.client.get(this.resource, { action: 'list_by_building', building: building });
    }

    /**
     * 根据类型查询教室
     * @param {string} type - 类型
     */
    async getByType(type) {
        return this.client.get(this.resource, { action: 'list_by_type', type: type });
    }

    /**
     * 根据状态查询教室
     * @param {string} status - 状态
     */
    async getByStatus(status) {
        return this.client.get(this.resource, { action: 'list_by_status', status: status });
    }
}

/**
 * 选课API
 */
class EnrollmentApi {
    constructor(client) {
        this.client = client;
        this.resource = '/enrollments';
    }

    /**
     * 学生选课
     * @param {string} studentId - 学生ID
     * @param {string} courseId - 课程ID
     */
    async enroll(studentId, courseId) {
        return this.client.post(this.resource, {
            student_id: studentId,
            course_id: courseId
        });
    }

    /**
     * 退课
     * @param {string} enrollmentId - 选课记录ID
     */
    async drop(enrollmentId) {
        return this.client.delete(`${this.resource}/${enrollmentId}`);
    }

    /**
     * 获取学生的选课列表
     * @param {string} studentId - 学生ID
     */
    async getStudentCourses(studentId) {
        return this.client.get(`/students/${studentId}/courses`);
    }
}

/**
 * 学校新闻API
 */
class NewsApi {
    constructor(client) {
        this.client = client;
        this.resource = '/news';
    }

    /**
     * 获取所有新闻
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_news', ...params });
    }

    /**
     * 获取已发布新闻
     */
    async getPublished() {
        return this.client.get(this.resource, { action: 'list_published' });
    }

    /**
     * 获取精选新闻
     */
    async getFeatured() {
        return this.client.get(this.resource, { action: 'list_featured' });
    }

    /**
     * 根据ID获取新闻
     * @param {string} id - 新闻ID
     */
    async getById(id) {
        return this.client.get(this.resource, { action: 'query_by_id', id: id });
    }

    /**
     * 创建新闻
     * @param {Object} data - 新闻数据
     */
    async create(data) {
        return this.client.post(this.resource, { action: 'add_news', ...data });
    }

    /**
     * 更新新闻
     * @param {string} id - 新闻ID
     * @param {Object} data - 新闻数据
     */
    async update(id, data) {
        return this.client.post(this.resource, { action: 'update_news', id: id, ...data });
    }

    /**
     * 删除新闻
     * @param {string} id - 新闻ID
     */
    async delete(id) {
        return this.client.post(this.resource, { action: 'delete_news', id: id });
    }

    /**
     * 发布新闻
     * @param {string} id - 新闻ID
     */
    async publish(id) {
        return this.client.post(this.resource, { action: 'publish_news', id: id });
    }

    /**
     * 增加浏览量
     * @param {string} id - 新闻ID
     */
    async incrementView(id) {
        return this.client.post(this.resource, { action: 'increment_view', id: id });
    }

    /**
     * 根据类别查询新闻
     * @param {string} category - 类别
     */
    async getByCategory(category) {
        return this.client.get(this.resource, { action: 'list_by_category', category: category });
    }

    /**
     * 根据作者查询新闻
     * @param {string} author_id - 作者ID
     */
    async getByAuthor(author_id) {
        return this.client.get(this.resource, { action: 'list_by_author', author_id: author_id });
    }
}

/**
 * 通知API
 */
class NotificationApi {
    constructor(client) {
        this.client = client;
        this.resource = '/notification';
    }

    /**
     * 获取所有通知
     * @param {Object} params - 查询参数
     */
    async getList(params = {}) {
        return this.client.get(this.resource, { action: 'list_notifications', ...params });
    }

    /**
     * 获取已发布通知
     */
    async getPublished() {
        return this.client.get(this.resource, { action: 'list_published_notifications' });
    }

    /**
     * 根据ID获取通知
     * @param {string} id - 通知ID
     */
    async getById(id) {
        return this.client.post(this.resource, {
            action: 'query_notification_by_id',
            id: id
        });
    }

    /**
     * 创建通知
     * @param {Object} data - 通知数据
     */
    async create(data) {
        return this.client.post(this.resource, {
            action: 'add_notification',
            ...data
        });
    }

    /**
     * 更新通知
     * @param {string} id - 通知ID
     * @param {Object} data - 通知数据
     */
    async update(id, data) {
        return this.client.post(this.resource, {
            action: 'update_notification',
            id: id,
            ...data
        });
    }

    /**
     * 删除通知
     * @param {string} id - 通知ID
     */
    async delete(id) {
        return this.client.post(this.resource, {
            action: 'delete_notification',
            id: id
        });
    }

    /**
     * 发布通知
     * @param {string} id - 通知ID
     */
    async publish(id) {
        return this.client.post(this.resource, {
            action: 'publish_notification',
            id: id
        });
    }
}

/**
 * 认证API
 */
class AuthApi {
    constructor(client) {
        this.client = client;
        this.resource = '/auth';
    }

    /**
     * 登录
     * @param {string} username - 用户名
     * @param {string} password - 密码
     */
    async login(username, password) {
        const response = await this.client.post(`${this.resource}/login`, {
            username,
            password
        });

        // 保存token
        if (response.data && response.data.token) {
            this.client.setToken(response.data.token);
        }

        return response;
    }

    /**
     * 登出
     */
    async logout() {
        this.client.setToken(null);
        // 可以添加调用后端登出接口
        return Promise.resolve();
    }

    /**
     * 获取当前用户信息
     */
    async getCurrentUser() {
        return this.client.get(`${this.resource}/me`);
    }
}

/**
 * API工厂类 - 统一管理所有API实例
 */
class ApiFactory {
    constructor(baseURL) {
        this.client = new BoostProApiClient(baseURL);
        this.student = new StudentApi(this.client);
        this.teacher = new TeacherApi(this.client);
        this.course = new CourseApi(this.client);
        this.classroom = new ClassroomApi(this.client);
        this.enrollment = new EnrollmentApi(this.client);
        this.news = new NewsApi(this.client);
        this.notification = new NotificationApi(this.client);
        this.auth = new AuthApi(this.client);
    }

    /**
     * 设置认证Token
     */
    setToken(token) {
        this.client.setToken(token);
    }

    /**
     * 获取认证Token
     */
    getToken() {
        return this.client.token;
    }
}

// 创建全局API实例 - 注意：实际API路径为 /api/student, /api/teacher, /api/course
// 使用相对路径，自动适配当前域名和端口
const api = new ApiFactory('/api');

// 导出到全局
window.BoostProApi = {
    api,
    BoostProApiClient,
    StudentApi,
    TeacherApi,
    CourseApi,
    ClassroomApi,
    NewsApi,
    EnrollmentApi,
    NotificationApi,
    AuthApi
};
