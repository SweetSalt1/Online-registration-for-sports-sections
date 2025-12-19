#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session {
public:
	void SetToken(const std::string& t) { token_ = t; }
	void SetRole(const std::string& r) { role_ = r; }

	const std::string& GetToken() const { return token_; }
	const std::string& GetRole() const { return role_; }

	bool IsLogged() const { return !token_.empty(); }

private:
	std::string token_;
	std::string role_;
};

#endif  
