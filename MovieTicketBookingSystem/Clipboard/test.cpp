#include "clipboard.h"
#include <string>
#include <format>


int main() {
	while (true) {
		int ch; std::cin >> ch;
		if (ch == 0) {
			std::cout << std::format("Text from clipboard is \"{}\"", readFromClipboard());
		}
		else {

			writeToClipboard("text inserted");
		}
	}
}