#include "app.h"
#include "session.h"

void App::adminPage() {
	bool running = true;

	auto user = currentSession->getUser();


	Dict<std::string, RedirectFunction> redirects = {
		{"Create", [this, &user]() -> void {}},
		{"Modify", [this, &user]() -> void {}},
		{"Delete", [this, &user]() -> void {}},
		{"<< Back", [this, &running]() -> void { running = false; }}
	};

	std::vector<std::string> actions;
	actions.reserve(redirects.size());

	for (const auto& redirect : redirects) {
		actions.emplace_back(redirect.first);
	}

	while (running) {
		size_t choice = menu->getChoice(actions, "Choose an action: ");

		redirects.at(choice).second();
	}
}