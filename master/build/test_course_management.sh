#!/bin/bash

# 课程管理功能测试脚本
# 测试增删改查功能

BASE_URL="http://127.0.0.1:20080/api"

echo "========== 课程管理功能测试 =========="
echo ""

# 1. 测试查询所有课程
echo "1. 测试查询所有课程..."
curl -s "$BASE_URL/course?action=list_courses" | python3 -m json.tool | head -50
echo ""
echo ""

# 2. 测试添加新课程（带时间安排）
echo "2. 测试添加新课程（带时间安排）..."
ADD_RESPONSE=$(curl -s -X POST "$BASE_URL/course" \
  -H "Content-Type: application/json" \
  -d '{
    "action": "add_course",
    "course_id": "CS202",
    "course_name": "操作系统原理",
    "teacher_id": "T001",
    "credits": 4,
    "course_type": "必修",
    "semester": "2024-2025-2",
    "max_students": 80,
    "description": "计算机专业核心课程",
    "schedules": [
      {
        "day_of_week": "周一",
        "start_time": "08:00",
        "end_time": "09:40",
        "location": "A楼301",
        "week_range": "1-16周"
      },
      {
        "day_of_week": "周三",
        "start_time": "10:00",
        "end_time": "11:40",
        "location": "A楼301",
        "week_range": "1-16周"
      }
    ]
  }')

echo "$ADD_RESPONSE" | python3 -m json.tool
NEW_COURSE_ID=$(echo "$ADD_RESPONSE" | python3 -c "import sys, json; data=json.load(sys.stdin); print(data.get('data', {}).get('id', ''))" 2>/dev/null)
echo "新课程ID: $NEW_COURSE_ID"
echo ""
echo ""

# 3. 查询刚添加的课程
if [ ! -z "$NEW_COURSE_ID" ]; then
    echo "3. 测试查询课程详情 (ID: $NEW_COURSE_ID)..."
    curl -s "$BASE_URL/course?action=query_course_by_id&id=$NEW_COURSE_ID" | python3 -m json.tool
    echo ""
    echo ""

    # 4. 测试更新课程
    echo "4. 测试更新课程..."
    curl -s -X POST "$BASE_URL/course" \
      -H "Content-Type: application/json" \
      -d "{
        \"action\": \"update_course\",
        \"id\": \"$NEW_COURSE_ID\",
        \"course_name\": \"操作系统原理（更新版）\",
        \"credits\": 5,
        \"description\": \"计算机专业核心课程 - 已更新\",
        \"schedules\": [
          {
            \"day_of_week\": \"周二\",
            \"start_time\": \"14:00\",
            \"end_time\": \"15:40\",
            \"location\": \"B楼201\",
            \"week_range\": \"1-18周\"
          }
        ]
      }" | python3 -m json.tool
    echo ""
    echo ""

    # 5. 再次查询确认更新
    echo "5. 查询更新后的课程..."
    curl -s "$BASE_URL/course?action=query_course_by_id&id=$NEW_COURSE_ID" | python3 -m json.tool
    echo ""
    echo ""

    # 6. 测试删除课程
    echo "6. 测试删除课程..."
    curl -s -X POST "$BASE_URL/course" \
      -H "Content-Type: application/json" \
      -d "{
        \"action\": \"delete_course\",
        \"id\": \"$NEW_COURSE_ID\"
      }" | python3 -m json.tool
    echo ""
    echo ""

    # 7. 确认删除
    echo "7. 确认课程已删除..."
    curl -s "$BASE_URL/course?action=query_course_by_id&id=$NEW_COURSE_ID" | python3 -m json.tool
else
    echo "无法获取新课程ID，跳过后续测试"
fi

echo ""
echo "========== 测试完成 =========="
