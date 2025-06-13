#pragma once

#include <iostream>
#include <vector>



template<typename F, typename S>
class Dict {
private:
	std::vector<std::pair<F, S>> data;

public:
	Dict() = default;

	std::pair<F, S>& at(const size_t index) {
		if (index < data.size()) {
			return data[index];
		}
		throw std::out_of_range("Index out of range");
	}


	const std::pair<F, S>& at(const size_t index) const {
		if (index < data.size()) {
			return data[index];
		}
		throw std::out_of_range("Index out of range");
	}

	void insert(const F& key, const S& value) {
		for (auto& pair : data) {
			if (pair.first == key) {
				pair.second = value;
				return;
			}
		}
		data.emplace_back(key, value);
	}

	S& operator[](const F& key) {
		for (auto& pair : data) {
			if (pair.first == key) {
				return pair.second;
			}
		}
		data.emplace_back(key, S{});
		return data.back().second;
	}

	const S& operator[](const F& key) const {
		for (const auto& pair : data) {
			if (pair.first == key) {
				return pair.second;
			}
		}
		throw std::out_of_range("Key not found in const Dict access");
	}

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }

	auto begin() const { return data.cbegin(); }
	auto end() const { return data.cend(); }
	auto cbegin() const { return data.cbegin(); }
	auto cend() const { return data.cend(); }

	size_t size() const {
		return data.size();
	}

	bool empty() const {
		return data.empty();
	}

	void clear() {
		data.clear();
	}
};