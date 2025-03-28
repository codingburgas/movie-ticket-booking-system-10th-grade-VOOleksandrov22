#pragma once

#include <iostream>
#include "config.h"
#include "../Database/dbWrapper.h"

class App {
private:
	

public:
	const Config* config = new Config();
	const DbWrapper* db;

	App();
};