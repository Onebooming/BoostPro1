#include "student.hpp"
#include "../../../public/common.h"
namespace chenglei {

/**
 * @brief 
 * 
 * @param np_student 调用时候需要进行移动语义转换
 * @return unsigned 
 *      ERROR_INVALID_PARAM： 不合法的参数
 *      ERROR_ALREADY_EXIST： 对象已经存在，无法加入全局管理器
 *      ERROR_SUCCESS： 成功加入全局管理器
 */
unsigned StudentMgr::addStudent(std::unique_ptr<StudentBaseInfo> np_student){
    if(nullptr == np_student) {
        return ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    const std::string &stuid = np_student->getStudentID();
    if (student_map_.find(stuid) != student_map_.end())
    {
        return ERROR_ALREADY_EXIST;
    }
    student_map_[stuid] = std::move(np_student);
    return ERROR_SUCCESS;
}

void StudentMgr::removeStudentByStuId(std::string & student_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    student_map_.erase(student_id); 
}

const StudentBaseInfo* StudentMgr::getStudentByStuId(std::string & student_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = student_map_.find(student_id);
    if(it != student_map_.end())
    {
        return it->second.get();
    }

    return nullptr; // todo coding
}

std::vector<StudentBaseInfo *> StudentMgr::getAllStudentList(void) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<StudentBaseInfo *> result;
    for(auto& item : student_map_) {
        result.push_back(item.second.get());
    }

    return result;
}

}