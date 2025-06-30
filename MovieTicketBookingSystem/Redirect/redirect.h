#pragma once

#include <iostream>
#include <functional>

enum MessageType {
	INFO,
	WARNING,
	ERROR,
	SUCCESS
};

class Redirect : std::exception {
private:
	std::string message;
	MessageType type;

public:
	std::function<void()> redirectFunction;

	Redirect(const std::string& message, const std::function<void()>& redirectFunction, const MessageType& type = MessageType::INFO) :
		std::exception(message.c_str()),
		message(message),
		type(type),
		redirectFunction(redirectFunction) {}

	std::string getMessage() const {
		return message;
	}
	MessageType getType() const {
		return type;
	}
	const char* what() const noexcept override {
		return message.c_str();
	}

	void print(const Redirect& r);

};
