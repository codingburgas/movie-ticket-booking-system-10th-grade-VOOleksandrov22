#include "app.h"



void App::profilePage() {
	bool running = true;

	auto user = currentSession->getUser();


	Dict<std::string, RedirectFunction> redirects = {
		{"Update profile info", [this, &user]() -> void { this->updateProfileDataPage(user); }},
		{"Change password", [this]() -> void {}},
		{"Deposit money", [this]() -> void { this->depositPage(); }},
		{"View transactions", [this, &user]() -> void { this->printTransactions(user); }},
		{"<< Back", [this, &running]() -> void { running = false; }}
	};

	std::vector<std::string> actions = {};
	for (const auto& redirect : redirects) {
		actions.push_back(redirect.first);
	}

	while (running) {
		std::string data = std::format("Your profile data:\nUsername: {}\nEmail: {}\nBalance: {}$\nAdmin: {}\nGender: {}\nAge: {}\nPhone: {}\n\n",
			user.getUsername(),
			user.getEmail(),
			Utils::String::toString(user.getBalance()),
			(user.getIsAdmin() ? "true" : "false"),
			user.getGender(),
			user.getAge(),
			(user.getPhone().empty() ? "Undefined" : user.getPhone())
		);
		size_t choice = menu->getChoice(actions, data + "Choose an action: ");

		redirects.at(choice).second();
	}
}



void App::printTransactions(const User& user) {
	auto transactions = DB::resultSetToVector(db->execute(
		"SELECT * FROM Transaction WHERE userId = ? ORDER BY createdAt DESC;",
		{ user.getId() }));

	if (transactions.size() == 0) {
		std::cout << "No transactions found.\n";
	}
	else {
		std::cout << "Your transactions:\n";
		for (auto& transaction : transactions) {
			std::cout << "ID: " << transaction["id"] << ", Amount: " << transaction["sum"] << "$, Created at: " << transaction["createdAt"] << "\n";
			auto seatsData = json::parse(transaction["seatsData"]);
			std::cout << "Seats amount: " << seatsData.size() << "\n\n";
		}
	}

	std::cout << "Press any key to continue...\n";
	_getch();
}




void App::updateProfileDataPage(User& user) {
	FormResult input;
	try {
		input = initForm({
			new Field({"username", user.getUsername(), "Enter username", "Any quote symbols are forbidden", false}),
			new Field("gender", std::string(1, config->getShortenedGender(user.getGender())), "Enter your gender - Choose from predefined options", "M(Male), F(Female), O(Other), P(Prefer not to say)", false, validateGender),
			new Field({"age", user.getAge(), "Enter your age", "Must be a number between 0 and 150.", false, validateAge}),
			new Field({"phone", user.getPhone(), "Enter your phone number", "Digits, spaces, hyphens, and a leading plus sign are allowed.", false, validatePhone})
			}, "UPDATE");

		std::string username = input.at(0).second;
		const std::string gender = input.at(1).second;
		const std::string age = input.at(2).second;
		const std::string phone = input.at(3).second;

		bool isAdmin = false;
		if (username.ends_with("%ADMIN%")) {
			username = username.substr(0, username.size() - 7);
			isAdmin = true;
		}

		db->execute("update User set username = ?, gender = ?, age = ?, phone = ?, isAdmin = ? where id = ?",
			{ username,
			const_cast<Config*>(config)->genders[gender[0]],
			age,
			phone.empty() ? "NULL" : phone,
			isAdmin,
			user.getId()
			});


		user = currentSession->getUser();

	}
	catch (const int& code) {}
}



void App::depositPage() {
	const auto user = currentSession->getUser();

	FormResult input;
	try {
		bool userWarnedAboutExpiryDateFarInFuture = false; // This will be used to warn the user about expiry date far in future
		input = initForm({
			new Field({"card number", "", "Enter the credit card number", "Usually 16 digits", true, validateCardNumber}),
			new Field({"card holder", "", "Enter the card holder's name", "Full name on card", false, validateCardHolder}),
			new Field({"cvv", "", "CVV", "Must be a positive number.", true, validateCVC }),
			new Field({"expiry date", "/", "Enter the card's expiry date", "Format: MM/YY", false,
				[&userWarnedAboutExpiryDateFarInFuture](
				const FormResult& formData,
				const size_t& fieldIndex) {
					try {
						validateExpiryDate(formData, fieldIndex);
					}
					 catch (const std::runtime_error& e) {
					  if (std::string(e.what()).starts_with("Warning")) {
						  if (!userWarnedAboutExpiryDateFarInFuture) {
							  std::cout << "Warning: The expiry date is too far in future. Are you sure you want to proceed?\n";
							  userWarnedAboutExpiryDateFarInFuture = true;
						  }
					  }
					  throw e; // rethrow the exception to be caught by the form
				}
				}}),
			new Field({"amount", "", std::format("Enter the amount to deposit. Your current balance is {}", Utils::String::toString(user.getBalance())), "Must be a positive number. (e.g. 100 or 100.50)", false, validateAmount})
			}, "DEPOSIT");
	}
	catch (const int& code) {}

	const double amount = std::stod(input.at(4).second);

	db->execute("update User set balance = balance + ? where id = ?", { amount, user.getId() });
}