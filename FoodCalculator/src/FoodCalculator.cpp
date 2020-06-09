#include "FoodCalculator.h"

#include "GenericVector.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ostream>
#include <chrono>
#include <random>

namespace plt = matplotlibcpp;

const std::string ASSET_PATH = "../../";

struct Food {
	int yearsToCalculate;

	float initialProt;
	float initialCarb;
	float initialFat;
	float initialKcal;

	float dailyKcalUsage;
	float dailyProtUsage;
	float dailyCarbUsage;
	float dailyFatUsage;

	bool printHarvestNames = true;
	bool printHuntNames = false;
	bool printGatherNames = true;
	bool printPlantNames = true;

	bool printKcal = true;
	bool printProt = true;
	bool printCarb = true;
	bool printFat = true;

	bool printSmooth = false;

	std::vector<float> days;
	std::vector<float> kcal;
	std::vector<float> prot;
	std::vector<float> carb;
	std::vector<float> fat;

	/* <============ FOOD BY AREA ===============>*/
	// read from csv
	struct Harvest {
		std::vector<std::string> namesFBA;
		GenericVector<float> areas;
		GenericVector<float> kgPerAreas;
		GenericVector<float> kcalPerKgs;
		GenericVector<float> gProtPerKg;
		GenericVector<float> gCarbPerKg;
		GenericVector<float> gFatPerKg;
		GenericVector<float> percReqReplant;
		GenericVector<float> harvestDays;
		// calculate stuff
		GenericVector<float> harvestCals;
		GenericVector<float> harvestProt;
		GenericVector<float> harvestCarb;
		GenericVector<float> harvestFat;
	} harvest;

	/* <============= FOOD BY HUNTING ============>*/
	struct Hunt {
		std::vector<std::string> namesFBH;
		GenericVector<float> avgkg;
		GenericVector<float> kcalPerKg;
		GenericVector<float> gProtPerKgs;
		GenericVector<float> gCarbPerKgs;
		GenericVector<float> gFatPerKgs;
		GenericVector<float> nHunts;
		GenericVector<float> isUniform;
		GenericVector<float> isNormal;
		GenericVector<float> mean;
		GenericVector<float> stddev;

		GenericVector<float> huntCals;
		GenericVector<float> huntProt;
		GenericVector<float> huntCarb;
		GenericVector<float> huntFat;
	} hunt;

	/* <============= FOOD BY GATHERING ============>*/
	struct Gathering {
		std::vector<std::string> namesFBG;
		GenericVector<float> avgkg;
		GenericVector<float> kcalPerKg;
		GenericVector<float> gProtPerKgs;
		GenericVector<float> gCarbPerKgs;
		GenericVector<float> gFatPerKgs;
		GenericVector<float> gatherDays;

		GenericVector<float> gatherCals;
		GenericVector<float> gatherProt;
		GenericVector<float> gatherCarb;
		GenericVector<float> gatherFat;

	} gather;

	struct Plants {
		std::vector<std::string> namesFBP;
		GenericVector<float> avgkg;
		GenericVector<float> kcalPerKg;
		GenericVector<float> gProtPerKgs;
		GenericVector<float> gCarbPerKgs;
		GenericVector<float> gFatPerKgs;
		GenericVector<float> nPlants;
		GenericVector<float> plantDays;

		GenericVector<float> plantCals;
		GenericVector<float> plantProt;
		GenericVector<float> plantCarb;
		GenericVector<float> plantFat;
	} plants;

};

std::vector<std::vector<int>> getRandomHuntDays(Food* food) {
	std::default_random_engine generator;
	std::vector<std::vector<int>> randomDays;
	randomDays.resize(food->hunt.nHunts.size());
	//Generate Hunt Random numbers
	for (int i = 0; i < food->hunt.nHunts.size(); ++i) {
		if ((int)food->hunt.isUniform[i]) {
			std::uniform_int_distribution<int> dist(1, 365);
			for (int j = 0; j < food->hunt.nHunts[i]; ++j) {
				randomDays[i].push_back(dist(generator));
			}
		}
		else if ((int)food->hunt.isNormal[i]) {
			int stddev = food->hunt.stddev[i];
			if (stddev <= 0) {
				stddev = 50;
			}
			std::normal_distribution<float> dist((float)food->hunt.mean[i], (float)stddev);
			for (int j = 0; j < food->hunt.nHunts[i]; ++j) {
				randomDays[i].push_back((int)dist(generator));
			}
		}

	}
	return randomDays;
}

float getTextRand(std::default_random_engine generator) {
	std::uniform_real_distribution<float> dist(-500, 500);
	return dist(generator);
}

void calcFoodYears(Food* food)
{
	float currentKcal = food->initialKcal;
	float currentProt = food->initialProt;
	float currentCarb = food->initialCarb;
	float currentFat = food->initialFat;

	std::default_random_engine generator;

	for (int i = 0; i < food->yearsToCalculate; ++i) {
		auto randomDays = getRandomHuntDays(food);
		for (int j = 0; j < 365; ++j) {
			int dayIndex = i * 365 + j;

			food->days[dayIndex] = dayIndex + 1;
			food->kcal[dayIndex] = currentKcal;
			food->prot[dayIndex] = currentProt;
			food->carb[dayIndex] = currentCarb;
			food->fat[dayIndex] = currentFat;

			//Harvest
			for (int k = 0; k < food->harvest.harvestDays.size(); ++k) {
				if (j == (int)food->harvest.harvestDays[k]) { // add a harvest of this type
					if (food->printHarvestNames) {
						plt::text(food->days[dayIndex], food->kcal[dayIndex] + getTextRand(generator), food->harvest.namesFBA[k]);
					}
					currentKcal += food->harvest.harvestCals[k];
					currentProt += food->harvest.harvestProt[k];
					currentCarb += food->harvest.harvestCarb[k];
					currentFat += food->harvest.harvestFat[k];
				}
			}
			//Hunt
			for (int k = 0; k < randomDays.size(); ++k) {
				for (int l = 0; l < randomDays[k].size(); ++l) {
					if (j == randomDays[k][l]) {
						if (food->printHuntNames) {
							plt::text(food->days[dayIndex], food->kcal[dayIndex] + getTextRand(generator), food->hunt.namesFBH[k]);
						}
						currentKcal += food->hunt.huntCals[k];
						currentProt += food->hunt.huntProt[k];
						currentCarb += food->hunt.huntCarb[k];
						currentFat += food->hunt.huntFat[k];
					}
				}
			}
			//Gather
			for (int k = 0; k < food->gather.gatherDays.size(); ++k) {
				if (j == (int)food->gather.gatherDays[k]) {
					if (food->printGatherNames) {
						plt::text(food->days[dayIndex], food->kcal[dayIndex] + getTextRand(generator), food->gather.namesFBG[k]);
					}
					currentKcal += food->gather.gatherCals[k];
					currentProt += food->gather.gatherProt[k];
					currentCarb += food->gather.gatherCarb[k];
					currentFat += food->gather.gatherFat[k];
				}
			}
			//Plants
			for (int k = 0; k < food->plants.plantDays.size(); ++k) {
				if (j == (int)food->plants.plantDays[k]) {
					if (food->printPlantNames) {
						plt::text(food->days[dayIndex], food->kcal[dayIndex] + getTextRand(generator), food->plants.namesFBP[k]);
					}
					currentKcal += food->plants.plantCals[k];
					currentProt += food->plants.plantProt[k];
					currentCarb += food->plants.plantCarb[k];
					currentFat += food->plants.plantFat[k];
				}
			}

			currentKcal -= food->dailyKcalUsage;
			currentProt -= food->dailyProtUsage;
			currentCarb -= food->dailyCarbUsage;
			currentFat -= food->dailyFatUsage;
		}
	}
}

void loadFoodByArea(Food* food, std::string filename) {
	rapidcsv::Document docFoodByArea(filename, rapidcsv::LabelParams(0, -1));

	food->harvest.namesFBA = docFoodByArea.GetColumn<std::string>("NAME");
	food->harvest.areas = docFoodByArea.GetColumn<float>("M2");
	food->harvest.kgPerAreas = docFoodByArea.GetColumn<float>("AVGKG/M2");
	food->harvest.kcalPerKgs = docFoodByArea.GetColumn<float>("KCAL/KG");
	food->harvest.gProtPerKg = docFoodByArea.GetColumn<float>("GPROT/KG");
	food->harvest.gCarbPerKg = docFoodByArea.GetColumn<float>("GCARB/KG");
	food->harvest.gFatPerKg = docFoodByArea.GetColumn<float>("GFAT/KG");
	food->harvest.percReqReplant = docFoodByArea.GetColumn<float>("PERC_REQ_REPLANT");
	food->harvest.harvestDays = docFoodByArea.GetColumn<float>("HARVEST_DAY");

	GenericVector<float> totalKgs = food->harvest.areas * food->harvest.kgPerAreas;
	food->harvest.harvestCals = totalKgs * food->harvest.kcalPerKgs * (1.0f - (food->harvest.percReqReplant / 100.0f));
	food->harvest.harvestProt = totalKgs * food->harvest.gProtPerKg;
	food->harvest.harvestCarb = totalKgs * food->harvest.gCarbPerKg;
	food->harvest.harvestFat= totalKgs * food->harvest.gFatPerKg;

}

void loadFoodByHunting(Food* food, std::string filename) {
	rapidcsv::Document docFoodByHunt(filename, rapidcsv::LabelParams(0, -1));

	food->hunt.namesFBH = docFoodByHunt.GetColumn<std::string>("NAME");
	food->hunt.avgkg = docFoodByHunt.GetColumn<float>("AVGKG");
	food->hunt.kcalPerKg = docFoodByHunt.GetColumn<float>("KCAL/KG");
	food->hunt.gProtPerKgs = docFoodByHunt.GetColumn<float>("GPROT/KG");
	food->hunt.gCarbPerKgs = docFoodByHunt.GetColumn<float>("GCARB/KG");
	food->hunt.gFatPerKgs = docFoodByHunt.GetColumn<float>("GFAT/KG");
	food->hunt.nHunts = docFoodByHunt.GetColumn<float>("N_HUNTS");
	food->hunt.isUniform = docFoodByHunt.GetColumn<float>("IS_UNIFORM");
	food->hunt.isNormal = docFoodByHunt.GetColumn<float>("IS_NORMAL");
	food->hunt.mean = docFoodByHunt.GetColumn<float>("MEAN");
	food->hunt.stddev = docFoodByHunt.GetColumn<float>("STDDEV");
	
	food->hunt.huntCals = food->hunt.avgkg * food->hunt.kcalPerKg;
	food->hunt.huntProt = food->hunt.avgkg * food->hunt.gProtPerKgs;
	food->hunt.huntCarb = food->hunt.avgkg * food->hunt.gCarbPerKgs;
	food->hunt.huntFat = food->hunt.avgkg * food->hunt.gFatPerKgs;

}

void loadFoodByGather(Food* food, std::string filename) {
	rapidcsv::Document docFoodByGather(filename, rapidcsv::LabelParams(0, -1));

	food->gather.namesFBG = docFoodByGather.GetColumn<std::string>("NAME");
	food->gather.avgkg = docFoodByGather.GetColumn<float>("AVGKG");
	food->gather.kcalPerKg = docFoodByGather.GetColumn<float>("KCAL/KG");
	food->gather.gProtPerKgs = docFoodByGather.GetColumn<float>("GPROT/KG");
	food->gather.gCarbPerKgs = docFoodByGather.GetColumn<float>("GCARB/KG");
	food->gather.gFatPerKgs = docFoodByGather.GetColumn<float>("GFAT/KG");
	food->gather.gatherDays = docFoodByGather.GetColumn<float>("GATHER_DAY");

	food->gather.gatherCals = food->gather.avgkg * food->gather.kcalPerKg;
	food->gather.gatherProt = food->gather.avgkg * food->gather.gProtPerKgs;
	food->gather.gatherCarb = food->gather.avgkg * food->gather.gCarbPerKgs;
	food->gather.gatherFat = food->gather.avgkg * food->gather.gFatPerKgs;
}

void loadFoodByPlants(Food* food, std::string filename) {
	rapidcsv::Document docFoodByPlants(filename, rapidcsv::LabelParams(0, -1));

	food->plants.namesFBP = docFoodByPlants.GetColumn<std::string>("NAME");
	food->plants.avgkg = docFoodByPlants.GetColumn<float>("AVGKG/PLANT");
	food->plants.kcalPerKg = docFoodByPlants.GetColumn<float>("KCAL/KG");
	food->plants.gProtPerKgs = docFoodByPlants.GetColumn<float>("GPROT/KG");
	food->plants.gCarbPerKgs = docFoodByPlants.GetColumn<float>("GCARB/KG");
	food->plants.gFatPerKgs = docFoodByPlants.GetColumn<float>("GFAT/KG");
	food->plants.nPlants = docFoodByPlants.GetColumn<float>("N_PLANTS");
	food->plants.plantDays = docFoodByPlants.GetColumn<float>("PLANTS_DAY");

	auto temp = food->plants.avgkg * food->plants.nPlants;
	food->plants.plantCals = temp * food->plants.kcalPerKg;
	food->plants.plantProt = temp * food->plants.gProtPerKgs;
	food->plants.plantCarb = temp * food->plants.gCarbPerKgs;
	food->plants.plantFat = temp * food->plants.gFatPerKgs;
}

void load(Food* food, std::string loadString) {
	std::string filename;
	bool loadedSomething = false;

	if (loadString.find("-FBA") != std::string::npos) {
		filename = ASSET_PATH + "food_config/food_by_area.csv";
		loadFoodByArea(food, filename);
		loadedSomething = true;
	}
	if (loadString.find("-FBH") != std::string::npos) {
		filename = ASSET_PATH + "food_config/food_by_hunt.csv";
		loadFoodByHunting(food, filename);
		loadedSomething = true;
	}
	if (loadString.find("-FBG") != std::string::npos) {
		filename = ASSET_PATH + "food_config/food_by_gather.csv";
		loadFoodByGather(food, filename);
		loadedSomething = true;
	}
	if (loadString.find("-FBP") != std::string::npos) {
		filename = ASSET_PATH + "food_config/food_by_plants.csv";
		loadFoodByPlants(food, filename);
		loadedSomething = true;
	}
	
	if (!loadedSomething) {
		std::cout << "Didn't load anything" << std::endl;
	}
}

void set(Food* food, std::string setString) {
	std::string s;
	std::string numberString = "";
	size_t findie;
	bool setSomething = false;

	// Prot
	{
		numberString = "";
		findie = setString.find("-init_prot");
		if (findie != std::string::npos) {
			s = setString.substr(findie + 11);
			if (!isdigit(s[0])) {
				std::cout << "Bad set format" << std::endl;
			}
			for (auto c : s) {
				if (isdigit(c)) {
					numberString += c;
				}
				else {
					break;
				}
			}
			int initProt = std::atoi(numberString.c_str());
			food->initialProt = initProt;
			setSomething = true;
			//std::cout << food->initialProt << " ";
		}
	}
	//Carb
	{
		numberString = "";
		findie = setString.find("-init_carb");
		if (findie != std::string::npos) {
			s = setString.substr(findie + 11);
			if (!isdigit(s[0])) {
				std::cout << "Bad set format" << std::endl;
			}
			for (auto c : s) {
				if (isdigit(c)) {
					numberString += c;
				}
				else {
					break;
				}
			}
			int initCarb = std::atoi(numberString.c_str());
			food->initialCarb = initCarb;
			setSomething = true;
			//std::cout << food->initialCarb << " ";
		}
	}
	//Fat
	{
		numberString = "";
		findie = setString.find("-init_fat");
		if (findie != std::string::npos) {
			s = setString.substr(findie + 10);
			if (!isdigit(s[0])) {
				std::cout << "Bad set format" << std::endl;
			}
			for (auto c : s) {
				if (isdigit(c)) {
					numberString += c;
				}
				else {
					break;
				}
			}
			int initFat = std::atoi(numberString.c_str());
			food->initialFat = initFat;
			setSomething = true;
			//std::cout << food->initialFat << " ";
		}
	}
	// Daily Usage
	{
		numberString = "";
		findie = setString.find("-daily_usage");
		if (findie != std::string::npos) {
			s = setString.substr(findie + 13);
			if (!isdigit(s[0])) {
				std::cout << "Bad set format" << std::endl;
			}
			for (auto c : s) {
				if (isdigit(c)) {
					numberString += c;
				}
				else {
					break;
				}
			}
			int dailyKcal = std::atoi(numberString.c_str());
			food->dailyKcalUsage = dailyKcal;
			food->dailyProtUsage = food->dailyKcalUsage * 0.2 / 4;
			food->dailyCarbUsage = food->dailyKcalUsage * 0.7 / 4;
			food->dailyFatUsage = food->dailyKcalUsage * 0.1 / 9;
			setSomething = true;
			//std::cout << food->dailyKcalUsage << " ";
		}
	}
	// Years to calculate
	{
		numberString = "";
		findie = setString.find("-years_to_calculate");
		if (findie != std::string::npos) {
			s = setString.substr(findie + 20);
			if (!isdigit(s[0])) {
				std::cout << "Bad set format" << std::endl;
			}
			for (auto c : s) {
				if (isdigit(c)) {
					numberString += c;
				}
				else {
					break;
				}
			}
			int yearsToCalculate = std::atoi(numberString.c_str());
			int daysToCalculate = 365 * yearsToCalculate;
			food->yearsToCalculate = yearsToCalculate;

			food->days.resize(daysToCalculate);
			food->prot.resize(daysToCalculate);
			food->carb.resize(daysToCalculate);
			food->fat.resize(daysToCalculate);
			food->kcal.resize(daysToCalculate);
			setSomething = true;
		}
	}
	//Print
	{
		//Kcal
		{
			numberString = "";
			findie = setString.find("-print_kcal");
			if (findie != std::string::npos) {
				s = setString.substr(findie + 12);
				if (!isdigit(s[0])) {
					std::cout << "Bad set format" << std::endl;
				}
				for (auto c : s) {
					if (isdigit(c)) {
						numberString += c;
					}
					else {
						break;
					}
				}
				int printKcal = std::atoi(numberString.c_str());
				food->printKcal = printKcal != 0;
				setSomething = true;
			}
		}
		//Prot
		{
			numberString = "";
			findie = setString.find("-print_prot");
			if (findie != std::string::npos) {
				s = setString.substr(findie + 12);
				if (!isdigit(s[0])) {
					std::cout << "Bad set format" << std::endl;
				}
				for (auto c : s) {
					if (isdigit(c)) {
						numberString += c;
					}
					else {
						break;
					}
				}
				int printProt = std::atoi(numberString.c_str());
				food->printProt = printProt != 0;
				setSomething = true;
			}
		}
		//Carb
		{
			numberString = "";
			findie = setString.find("-print_carb");
			if (findie != std::string::npos) {
				s = setString.substr(findie + 12);
				if (!isdigit(s[0])) {
					std::cout << "Bad set format" << std::endl;
				}
				for (auto c : s) {
					if (isdigit(c)) {
						numberString += c;
					}
					else {
						break;
					}
				}
				int printCarb = std::atoi(numberString.c_str());
				food->printCarb = printCarb != 0;
				setSomething = true;
			}
		}
		//Fat
		{
			numberString = "";
			findie = setString.find("-print_fat");
			if (findie != std::string::npos) {
				s = setString.substr(findie + 11);
				if (!isdigit(s[0])) {
					std::cout << "Bad set format" << std::endl;
				}
				for (auto c : s) {
					if (isdigit(c)) {
						numberString += c;
					}
					else {
						break;
					}
				}
				int printFat = std::atoi(numberString.c_str());
				food->printFat = printFat != 0;
				setSomething = true;
			}
		}
		//Smoothened
		{
			numberString = "";
			findie = setString.find("-print_smooth");
			if (findie != std::string::npos) {
				s = setString.substr(findie + 14);
				if (!isdigit(s[0])) {
					std::cout << "Bad set format" << std::endl;
				}
				for (auto c : s) {
					if (isdigit(c)) {
						numberString += c;
					}
					else {
						break;
					}
				}
				int printSmooth = std::atoi(numberString.c_str());
				food->printSmooth = printSmooth != 0;
				setSomething = true;
			}
		}
	}

	food->initialKcal = (4 * food->initialProt) + (4 * food->initialCarb) + (9 * food->initialFat);

	if (!setSomething) {
		std::cout << "Bad set format" << std::endl;
	}
}

template <class T>
std::vector<T> smoothed(std::vector<T>& v) {
	std::vector<T> ret;
	ret.resize(v.size());
	int blurSize = 10;
	if (v.size() < 3 * blurSize) {
		return v;
	}
	for (int i = 0; i < blurSize; ++i) {
		ret[i] = v[i];
	}
	for (int i = blurSize; i < v.size() - blurSize; ++i) {
		for (int j = 1; j < blurSize; ++j) {
			ret[i] += v[i - j];
			ret[i] += v[i + j];
		}
		ret[i] += v[i];
		ret[i] /= (float)2*(blurSize + 1);
	}
	for (int i = 1; i < blurSize + 1; ++i) {
		ret[v.size() - i] = v[v.size() - i];
	}
	return ret;
}

void rebuild(Food* food) {

	std::cout << food->harvest.areas.sum() << std::endl;
	//plt::xkcd();
	plt::figure_size(1200, 780);
	plt::axis("auto");

	calcFoodYears(food);

	if (food->printSmooth) {
		if (food->printKcal) {
			plt::named_plot("kcal", food->days, smoothed(food->kcal), "c-");
		}
		if (food->printProt) {
			plt::named_plot("prot", food->days, smoothed(food->prot), "r-");
		}
		if (food->printCarb) {
			plt::named_plot("carb", food->days, smoothed(food->carb), "g-");
		}
		if (food->printFat) {
			plt::named_plot("fat", food->days, smoothed(food->fat), "b-");
		}
	}
	else {
		if (food->printKcal) {
			plt::named_plot("kcal", food->days, food->kcal, "c-");
		}
		if (food->printProt) {
			plt::named_plot("prot", food->days, food->prot, "r-");
		}
		if (food->printCarb) {
			plt::named_plot("carb", food->days, food->carb, "g-");
		}
		if (food->printFat) {
			plt::named_plot("fat", food->days, food->fat, "b-");
		}
	}
	plt::legend();
	plt::show(true);
}

int main() {
	bool printHarvestNames = false;
	bool printHuntNames = false;
	bool printGatherNames = false;
	bool printPlantNames = false;

	int initalProt = 80 * 1000;
	int initalCarb = 800 * 1000;
	int initalFat = 20 * 1000;
	int dailyUsage = 8 * 1000;
	int yearsToCalculate = 4;

	Food food;

	std::string loadString = "-FBA -FBH -FBG -FBP";
	load(&food, loadString);

	std::string setString = "-init_prot " + std::to_string(initalProt) + 
		" -init_carb " + std::to_string(initalCarb) + 
		" -init_fat " + std::to_string(initalFat) + 
		" -daily_usage " + std::to_string(dailyUsage) +
		" -years_to_calculate " + std::to_string(yearsToCalculate);
	set(&food, setString);

	rebuild(&food);

	std::cout << "Total area from harvest by area: " << sum(food.harvest.areas) << std::endl;

	std::string str;
	std::getline(std::cin, str);
	while (str != "exit") {
		std::string tempStr = "";
		bool commandOk = false;
		for (auto c : str) {
			tempStr += c;
			if (tempStr == "load") {
				if (str.length() < 6) {
					break;
				}
				load(&food, str.substr(5));
				rebuild(&food);
				commandOk = true;
				break;
			}
			else if (tempStr == "set") {
				if (str.length() < 5) {
					break;
				}
				set(&food, str.substr(4));
				rebuild(&food);
				commandOk = true;
				break;
			}
		}
		if (!commandOk) {
			std::cout << "Invalid command" << std::endl;
		}

		std::getline(std::cin, str);
	}

	return 0;

}