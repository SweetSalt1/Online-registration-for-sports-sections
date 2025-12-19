#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

struct SectionInfo {
	int id;
	std::string name;
	std::string description;
};

struct SectionsResponse {
	std::vector<SectionInfo> sections;
};

struct LoginResponse {
	bool success = false;
	std::string token;
	std::string role;  // "student" or "secretary"
};

struct QueueItem {
	int student_id;
	std::string student_name;
	std::string section_name;
	std::string status;
};

struct QueueResponse {
	std::vector<QueueItem> list;
};

struct Notification {
	std::string name;
	std::string description;
	std::string timestamp;
};

struct NotificationsResponse {
	std::vector<Notification> list;
};

struct ApplicationHistoryItem {
	int section_id;
	std::string section_name;
	std::string status;
};

struct ApplicationsHistoryResponse {
	std::vector<ApplicationHistoryItem> list;
};

#endif  
