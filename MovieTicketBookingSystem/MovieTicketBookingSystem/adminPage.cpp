#include "app.h"
#include "session.h"
#include "../Validation/validation.h"
#include "../Redirect/redirect.h"


enum class Action {
	CREATE,
	MODIFY,
	DELETE,
	NONE
};

enum class DataToAlter {
	CINEMA,
	HALL,
	MOVIE,
	USER,
	NONE
};


std::string actionToStr(const Action& action) {
	switch (action)
	{
	case Action::CREATE:
		return "create";
	case Action::MODIFY:
		return "modify";
	case Action::DELETE:
		return "delete";
	default:
		return "error";
	}
}

void App::adminPage() {


	Dict<std::string, Action> actionOptions = {
		{"Create", Action::CREATE},
		{"Modify", Action::MODIFY},
		{"Delete", Action::DELETE},
		{"<< Back", Action::NONE}
	};

	
	Dict<std::string, DataToAlter> dataToAlterOptions = {
		{"Cinema", DataToAlter::CINEMA},
		{"Hall", DataToAlter::HALL},
		{"Movie", DataToAlter::MOVIE},
		{"User", DataToAlter::USER},
		{"<< Back", DataToAlter::NONE}
	};

	bool running = true;

	while (running) {
		system("cls");
		size_t choice = menu->getChoice(actionOptions.keys(), "Choose an action: ");
		const Action& action = actionOptions.at(choice).second;

		if (action == Action::NONE) break;

		choice = menu->getChoice(dataToAlterOptions.keys(), "Choose what data to alter: ");
		const DataToAlter& dataToAlter = dataToAlterOptions.at(choice).second;

		if (dataToAlter == DataToAlter::NONE) {
			running = false;
			continue;
		}

		switch (action) {
		case Action::CREATE:
			switch (dataToAlter) {
			case DataToAlter::CINEMA: createCinema(); break;
			}
			break;
		//	case DataToAlter::HALL:   createHall();   break;
		//	case DataToAlter::MOVIE:  createMovie();  break;
		//	case DataToAlter::USER:   createUser();   break;
		//	default: /* Should not happen if NONE is handled above */ break;
		//	}
		break; // Break from outer switch (Action::CREATE)

		case Action::MODIFY:
		switch (dataToAlter) {
		case DataToAlter::CINEMA: updateCinema(); break;
		//	case DataToAlter::HALL:   updateHall();   break;
		//	case DataToAlter::MOVIE:  updateMovie();  break;
		//	case DataToAlter::USER:   updateUser();   break;
		default: break;
		}
		break; // Break from outer switch (Action::MODIFY)

		case Action::DELETE:
		switch (dataToAlter) {
		//	case DataToAlter::CINEMA: deleteCinema(); break;
		//	case DataToAlter::HALL:   deleteHall();   break;
		//	case DataToAlter::MOVIE:  deleteMovie();  break;
		//	case DataToAlter::USER:   deleteUser();   break;
		default: break;
		}
		break; // Break from outer switch (Action::DELETE)

		case Action::NONE:
		//	// This case should be handled by the 'if (action == Action::NONE) break;' above
		//	// but a default or explicit break here is fine.
		running = false;
		break;

		default:
			// Fallback for any unhandled Action values
			std::cout << "Invalid action selected.\n";
			running = false; // Exit on unexpected action
			break;
		}
	}
}


void App::createCinema() {
	FormResult input;
	try {
		input = initForm({
			new Field({"Name", "", "Enter the name of the cinema", "", false, notEmpty}),
			new Field({"City", "", "Enter the name of the city", "", false, notEmpty}),
			new Field({"Address", "", "Enter the address of the cinema", "", false, notEmpty}),
			}, "CREATE");
	}
	catch (const int& code) {
		throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
	}

	const std::string& name = input.at(0).second;
	const std::string& city = input.at(1).second;
	const std::string& address = input.at(2).second;

	db->execute("insert into Cinema(name, city, street) values(?, ?, ?);", { name, city, address });
}


void App::updateCinema() {
	try {
		const unsigned long cinemaId = chooseCinemaMenu(chooseCityMenu());
		
		Row cinemaData = DB::resultSetToVector(db->execute("select * from Cinema where id = ?;", { cinemaId }))[0];

		FormResult input;
		try {
			input = initForm({
				new Field({"Name", cinemaData["name"], "Enter the name of the cinema", "", false, notEmpty}),
				new Field({"City", cinemaData["city"], "Enter the name of the city", "", false, notEmpty}),
				new Field({"Address", cinemaData["street"], "Enter the address of the cinema", "", false, notEmpty}),
				}, "UPDATE");
		}
		catch (const int& code) {
			throw Redirect("Form submission was cancelled\n\n", [this]() -> void { this->adminPage(); }, MessageType::WARNING);
		}

		const std::string& name = input.at(0).second;
		const std::string& city = input.at(1).second;
		const std::string& address = input.at(2).second;

		db->execute("update Cinema set name = ?, city = ?, street = ? where id = ?;", { name, city, address, cinemaId });
	}
	catch (const Redirect& redirect) {
		redirect.print();
		redirect.redirectFunction();
	}

}

