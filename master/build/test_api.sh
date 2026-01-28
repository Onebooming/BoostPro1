#!/bin/bash

# API 测试脚本
# 测试 /api/student, /api/teacher, /api/course 的增删改查功能

BASE_URL="http://127.0.0.1:20080"
SERVER_PID=""

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印函数
print_test() {
    echo -e "${YELLOW}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

# 检查服务器是否运行
check_server() {
    print_info "检查服务器状态..."
    if ! curl -s "$BASE_URL" > /dev/null 2>&1; then
        print_error "服务器未运行，请先启动 boost_server"
        exit 1
    fi
    print_success "服务器运行中"
}

# ==================== Student API 测试 ====================
test_student_get_all() {
    print_test "测试: GET /api/student - 查询所有学生"

    response=$(curl -s -X GET "$BASE_URL/api/student")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "查询所有学生成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_student_get_by_id() {
    print_test "测试: GET /api/student?id=xxx - 按ID查询学生"

    response=$(curl -s -X GET "$BASE_URL/api/student?id=1")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按ID查询学生成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_student_get_by_name() {
    print_test "测试: GET /api/student?name=xxx - 按姓名查询学生"

    response=$(curl -s -X GET "$BASE_URL/api/student?name=张")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按姓名查询学生成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_student_add() {
    print_test "测试: POST /api/student - 添加学生"

    student_data='{
        "name": "测试学生",
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
    }'

    response=$(curl -s -X POST "$BASE_URL/api/student" \
        -H "Content-Type: application/json" \
        -d "$student_data")

    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "添加学生成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

# ==================== Teacher API 测试 ====================
test_teacher_get_all() {
    print_test "测试: GET /api/teacher - 查询所有教师"

    response=$(curl -s -X GET "$BASE_URL/api/teacher")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "查询所有教师成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_teacher_get_by_id() {
    print_test "测试: GET /api/teacher?id=xxx - 按ID查询教师"

    response=$(curl -s -X GET "$BASE_URL/api/teacher?id=1")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按ID查询教师成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_teacher_get_by_name() {
    print_test "测试: GET /api/teacher?name=xxx - 按姓名查询教师"

    response=$(curl -s -X GET "$BASE_URL/api/teacher?name=李")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按姓名查询教师成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_teacher_add() {
    print_test "测试: POST /api/teacher - 添加教师"

    teacher_data='{
        "name": "测试教师",
        "birth_date": "1980-05-15",
        "address": "上海市",
        "gender": "男",
        "hobby": "科研",
        "phone_number": "13900139000",
        "father_name": "父亲",
        "mother_name": "母亲",
        "department": "计算机学院",
        "position": "教授",
        "office": "A101",
        "teacher_id": "T20240001"
    }'

    response=$(curl -s -X POST "$BASE_URL/api/teacher" \
        -H "Content-Type: application/json" \
        -d "$teacher_data")

    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "添加教师成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_teacher_update() {
    print_test "测试: PUT /api/teacher - 更新教师"

    teacher_data='{
        "id": "1",
        "name": "更新后的教师",
        "birth_date": "1980-05-15",
        "address": "北京市",
        "gender": "男",
        "department": "计算机学院",
        "position": "副教授",
        "office": "B202",
        "teacher_id": "T20240001"
    }'

    response=$(curl -s -X PUT "$BASE_URL/api/teacher" \
        -H "Content-Type: application/json" \
        -d "$teacher_data")

    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "更新教师成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_teacher_delete() {
    print_test "测试: DELETE /api/teacher?id=xxx - 删除教师"

    response=$(curl -s -X DELETE "$BASE_URL/api/teacher?id=999")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "删除教师请求成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

# ==================== Course API 测试 ====================
test_course_get_all() {
    print_test "测试: GET /api/course - 查询所有课程"

    response=$(curl -s -X GET "$BASE_URL/api/course")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "查询所有课程成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_course_get_by_id() {
    print_test "测试: GET /api/course?id=xxx - 按ID查询课程"

    response=$(curl -s -X GET "$BASE_URL/api/course?id=1")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按ID查询课程成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_course_get_by_name() {
    print_test "测试: GET /api/course?name=xxx - 按名称查询课程"

    response=$(curl -s -X GET "$BASE_URL/api/course?name=数学")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "按名称查询课程成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_course_add() {
    print_test "测试: POST /api/course - 添加课程"

    course_data='{
        "course_id": "C20240001",
        "course_name": "测试课程",
        "teacher_id": "T20240001",
        "credits": 3.0,
        "max_students": 100,
        "current_students": 0,
        "course_type": "必修",
        "semester": "2024-春季",
        "description": "这是一门测试课程"
    }'

    response=$(curl -s -X POST "$BASE_URL/api/course" \
        -H "Content-Type: application/json" \
        -d "$course_data")

    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "添加课程成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_course_update() {
    print_test "测试: PUT /api/course - 更新课程"

    course_data='{
        "id": "1",
        "course_id": "C20240001",
        "course_name": "更新后的课程",
        "teacher_id": "T20240001",
        "credits": 4.0,
        "max_students": 120,
        "current_students": 0,
        "course_type": "选修",
        "semester": "2024-秋季",
        "description": "这是更新后的课程描述"
    }'

    response=$(curl -s -X PUT "$BASE_URL/api/course" \
        -H "Content-Type: application/json" \
        -d "$course_data")

    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "更新课程成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

test_course_delete() {
    print_test "测试: DELETE /api/course?id=xxx - 删除课程"

    response=$(curl -s -X DELETE "$BASE_URL/api/course?id=999")
    echo "$response" | python3 -m json.tool > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "删除课程请求成功"
        echo "$response" | python3 -m json.tool
    else
        print_error "返回的不是有效的JSON"
        echo "$response"
    fi
    echo ""
}

# ==================== 主测试流程 ====================
main() {
    echo "========================================="
    echo "       BoostPro API 测试脚本"
    echo "========================================="
    echo ""

    check_server
    echo ""

    # Student API 测试
    echo "========================================="
    echo "        Student API 测试"
    echo "========================================="
    test_student_get_all
    test_student_get_by_id
    test_student_get_by_name
    test_student_add

    # Teacher API 测试
    echo "========================================="
    echo "        Teacher API 测试"
    echo "========================================="
    test_teacher_get_all
    test_teacher_get_by_id
    test_teacher_get_by_name
    test_teacher_add
    test_teacher_update
    test_teacher_delete

    # Course API 测试
    echo "========================================="
    echo "        Course API 测试"
    echo "========================================="
    test_course_get_all
    test_course_get_by_id
    test_course_get_by_name
    test_course_add
    test_course_update
    test_course_delete

    echo "========================================="
    echo "        所有测试完成"
    echo "========================================="
}

# 运行主函数
main
