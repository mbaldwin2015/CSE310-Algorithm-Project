/*
 * Michael Baldwin
 * CSE310 Assignment 1
 * Spring 2016
 */

#include <iostream>
#include <sstream>
#include <string>
#include <omp.h>

using namespace std;

struct team_stats {  // Struct to hold all team data for a year
	char team_name[21];		// Name of NFL team
	int games;				// Number of games played in the season
	float pts_per_game;		// Points per game
	int total_points;		// Total points
	int scrimmage_plays;	// Scrimmage plays
	float yds_per_game;		// Yards per game
	float yds_per_play;		// Yards per play
	float first_per_game;	// First downs per game
	int third_md;			// Third down conversions
	int third_att;			// Third down attempts
	int third_pct;			// Third down percentage
	int fourth_md;			// Fourth down conversions
	int fourth_att;			// Fourth down attempts
	int fourth_pct;			// Fourth down percentage
	int penalties;			// Number of penalties
	int pen_yds;			// Penalty yards
	char top_per_game[5];	// Time of possession per game
	int fum;				// Number of fumbles
	int lost;				// Fumbles lost
	int to;					// Turnover ratio
	int year;
};

struct annual_stats {  // Struct to hold an array of team data structs for a year
	int year;
	struct team_stats teams[32];
};

// Forward declarations
void bSortRange(annual_stats*, int, int, string, string, int);
void bSortYear(annual_stats*, int, string, string, int);
void bFind(annual_stats*, int, string, string, int);
void bFindPrint(annual_stats*, string, string, int);
bool bSortSwap(annual_stats*, int, int, string, string, string);
bool bSortSwap(annual_stats*, int, int, int, int, string);
bool bSortSwap(annual_stats*, int, int, float, float, string);
bool bSortSwap(team_stats*, int, string, string, string);
bool bSortSwap(team_stats*, int, int, int, string);
bool bSortSwap(team_stats*, int, float, float, string);
void quickSortName(annual_stats*, int, int, int, string);
int quickSortNamePartition(annual_stats*, int, int, int, string);
void quickSortTOP(annual_stats*, int, int, int, string);
void printArray(annual_stats*, int);

// Calls sorting algorithm function.
template <typename T, typename M> void sort_by_field(T a[], int n, const M T::*p, string order) {
	quickSort(a, 0, n - 1, p, order);
}

// Calls printing function.
template <typename T, typename M> void sort_by_field(T a[], const M T::*p) {
	quickSortPrint(a, p);
}

// Calls sorting algorithm function.
template <typename T, typename M> void range_print(T a[], int n, const M T::*p) {
	quickSortRangePrint(a, p, n);
}

// Calls computerAverage function.
template <typename T, typename M> float compute_average(T a[], const M T::*p) {
	return computeAverage(a, p);
}

/* 
 *  MAIN BEGINS
 *  ALL INPUT IS TAKEN FROM FILE REDIRECTION TO STANARD INPUT
 */
int main() {
	int numYears, numCommands, startYear, endYear;
	string sortType, field, order, range;
	cin >> numYears;

	annual_stats* teamData = new annual_stats[numYears]; // Dynamically allocate memory for the array of structs

	// Fill all of the team data from the file
	for (int i = 0; i < numYears; i++) {
		cin >> teamData[i].year;
		for (int j = 0; j < 32; j++) {
			cin.ignore(2);
			cin.getline(teamData[i].teams[j].team_name, 21, '"');
			cin >> teamData[i].teams[j].games;
			cin >> teamData[i].teams[j].pts_per_game;
			cin >> teamData[i].teams[j].total_points;
			cin >> teamData[i].teams[j].scrimmage_plays;
			cin >> teamData[i].teams[j].yds_per_game;
			cin >> teamData[i].teams[j].yds_per_play;
			cin >> teamData[i].teams[j].first_per_game;
			cin >> teamData[i].teams[j].third_md;
			cin >> teamData[i].teams[j].third_att;
			cin >> teamData[i].teams[j].third_pct;
			cin >> teamData[i].teams[j].fourth_md;
			cin >> teamData[i].teams[j].fourth_att;
			cin >> teamData[i].teams[j].fourth_pct;
			cin >> teamData[i].teams[j].penalties;
			cin >> teamData[i].teams[j].pen_yds;
			cin.ignore(2);
			cin.getline(teamData[i].teams[j].top_per_game, 6, '"');
			cin >> teamData[i].teams[j].fum;
			cin >> teamData[i].teams[j].lost;
			cin >> teamData[i].teams[j].to;
		}
	}

	cin >> numCommands;

	omp_set_nested(1);
	omp_set_num_threads(2);

	// Parse commands
	for (int i = 0; i < numCommands; i++) {
		cin >> sortType;
		if (sortType == "bsort") { 
			cin >> range;
			if (range == "range") { // bsort range commands
				cin >> startYear;
				cin >> endYear;
				cin >> field;
				cin >> order;
				cout << "\n" << sortType << " " << range << " " << startYear << " " << endYear << " " << field << " " << order << "\n";
				bSortRange(teamData, startYear, endYear, field, order, numYears);
			}
			else { // bsort year commands
				istringstream ( range ) >> startYear; // Only reason for <sstream> import, to convert string to int in linux
				cin >> field;
				cin >> order;
				cout << "\n" << sortType << " " << startYear << " " << field << " " << order << "\n"; // print command
				bSortYear(teamData, startYear, field, order, numYears);
			}
		}
		else if (sortType == "bfind") { // bfind commands
			cin >> startYear;
			cin >> field;
			cin >> order;
			cout << "\n" << sortType << " " << startYear << " " << field << " " << order << "\n"; // print command
			bFind(teamData, startYear, field, order, numYears);
		}
		else if (sortType == "qsort") { 
			cin >> range;
			if (range == "range") { // qsort range commands
				cin >> startYear;
				cin >> endYear;
				cin >> field;
				cin >> order;

				cout << "\n" << sortType << " " << range << " " << startYear << " " << endYear << " " << field << " " << order << "\n"; // print command

				int i = 0, j = 0;
				while (teamData[i].year != startYear) i++;
				while (teamData[j].year != endYear) j++;

				int totalNumStats = (j - i + 1)*32;

				team_stats* allYears = new team_stats[totalNumStats];

				// collection of all data across given range for sorting
				int m = 0;
				for (int k = i; k <= j; k++) {
					for (int l = 0; l < 32; l++) {
						allYears[m] = teamData[k].teams[l];
						allYears[m].year = teamData[k].year;
						m++;
					}
				}
				
				// key for switch statement
				int key;
				if (field == "team_name") key = 1;
				else if (field == "pts_per_game") key = 2;
				else if (field == "total_points") key = 3;
				else if (field == "scrimmage_plays") key = 4;
				else if (field == "yds_per_game") key = 5;
				else if (field == "yds_per_play") key = 6;
				else if (field == "first_per_game") key = 7;
				else if (field == "third_md") key = 8;
				else if (field == "third_att") key = 9;
				else if (field == "third_pct") key = 10;
				else if (field == "fourth_md") key = 11;
				else if (field == "fourth_att") key = 12;
				else if (field == "fourth_pct") key = 13;
				else if (field == "penalties") key = 14;
				else if (field == "pen_yds") key = 15;
				else if (field == "fum") key = 16;
				else if (field == "lost") key = 17;
				else if (field == "to") key = 18;
				else cout << "Field not recognized.\n";

				switch (key) {
				case 1:
					quickSortName(teamData, 0, totalNumStats-1, i, order);
					break;
				case 2:
					sort_by_field(allYears, totalNumStats, &team_stats::pts_per_game, order);
					cout << "Team\t\t\t" << "Points Per Game\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::pts_per_game);
					break;
				case 3:
					sort_by_field(allYears, totalNumStats, &team_stats::total_points, order);
					cout << "Team\t\t\t" << "Total Points\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::total_points);
					break;
				case 4:
					sort_by_field(allYears, totalNumStats, &team_stats::scrimmage_plays, order);
					cout << "Team\t\t\t" << "Scrimmage Plays\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::scrimmage_plays);
					break;
				case 5:
					sort_by_field(allYears, totalNumStats, &team_stats::yds_per_game, order);
					cout << "Team\t\t\t" << "Yards Per Game\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::yds_per_game);
					break;
				case 6:
					sort_by_field(allYears, totalNumStats, &team_stats::yds_per_play, order);
					cout << "Team\t\t\t" << "Yards Per Play\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::yds_per_play);
					break;
				case 7:
					sort_by_field(allYears, totalNumStats, &team_stats::first_per_game, order);
					cout << "Team\t\t\t" << "1st Downs per Game\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::first_per_game);
					break;
				case 8:
					sort_by_field(allYears, totalNumStats, &team_stats::third_md, order);
					cout << "Team\t\t\t" << "3rd Down Conversions\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::third_md);
					break;
				case 9:
					sort_by_field(allYears, totalNumStats, &team_stats::third_att, order);
					cout << "Team\t\t\t" << "3rd Down Attempts\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::third_att);
					break;
				case 10:
					sort_by_field(allYears, totalNumStats, &team_stats::third_pct, order);
					cout << "Team\t\t\t" << "3rd Down Percent\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::third_pct);
					break;
				case 11:
					sort_by_field(allYears, totalNumStats, &team_stats::fourth_md, order);
					cout << "Team\t\t\t" << "4th Down Conversions\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::fourth_md);
					break;
				case 12:
					sort_by_field(allYears, totalNumStats, &team_stats::fourth_att, order);
					cout << "Team\t\t\t" << "4th Down Attempts\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::fourth_att);
					break;
				case 13:
					sort_by_field(allYears, totalNumStats, &team_stats::fourth_pct, order);
					cout << "Team\t\t\t" << "4th Down Percent\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::fourth_pct);
					break;
				case 14:
					sort_by_field(allYears, totalNumStats, &team_stats::penalties, order);
					cout << "Team\t\t\t" << "Penalties\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::penalties);
					break;
				case 15:
					sort_by_field(allYears, totalNumStats, &team_stats::pen_yds, order);
					cout << "Team\t\t\t" << "Yards From Penalties\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::pen_yds);
					break;
				case 16:
					sort_by_field(allYears, totalNumStats, &team_stats::fum, order);
					cout << "Team\t\t\t" << "Fumbles\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::fum);
					break;
				case 17:
					sort_by_field(allYears, totalNumStats, &team_stats::lost, order);
					cout << "Team\t\t\t" << "Fumbles Lost\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::lost);
					break;
				case 18:
					sort_by_field(allYears, totalNumStats, &team_stats::to, order);
					cout << "Team\t\t\t" << "Turnover Ratio\t" << "Year\n";
					range_print(allYears, totalNumStats, &team_stats::to);
					break;
				default:
					cout << "Field not regocnized... again...\n";
					break;
				}
				delete allYears;
				cout << "\n";
			}
			else { // qsort year commands
				istringstream(range) >> startYear;
				cin >> field;
				cin >> order;

				int i = 0;
				while (teamData[i].year != startYear) i++;

				cout << "\n" << sortType << " " << startYear << " " << field << " " << order << "\n"; // print command

				if (field == "team_name") {
					quickSortName(teamData, 0, 31, i, order);
					cout << "Team\n";
					for (int j = 0; j < 32; j++)
						cout << "\"" << teamData[i].teams[j].team_name << "\"\n";
				}
				else if (field == "pts_per_game") {
					sort_by_field(teamData[i].teams, 32, &team_stats::pts_per_game, order); // call to sort
					cout << "Team\t\t\t" << "Points Per Game\n";
					sort_by_field(teamData[i].teams, &team_stats::pts_per_game); // call to print array
				}
				else if (field == "total_points") {
					sort_by_field(teamData[i].teams, 32, &team_stats::total_points, order);
					cout << "Team\t\t\t" << "Total Points\n";
					sort_by_field(teamData[i].teams, &team_stats::total_points);
				}
				else if (field == "scrimmage_plays") {
					sort_by_field(teamData[i].teams, 32, &team_stats::scrimmage_plays, order);
					cout << "Team\t\t\t" << "Plays From Scrimmage\n";
					sort_by_field(teamData[i].teams, &team_stats::scrimmage_plays);
				}
				else if (field == "yds_per_game") {
					sort_by_field(teamData[i].teams, 32, &team_stats::yds_per_game, order);
					cout << "Team\t\t\t" << "Yards Per Game\n";
					sort_by_field(teamData[i].teams, &team_stats::yds_per_game);
				}
				else if (field == "yds_pet_play") {
					sort_by_field(teamData[i].teams, 32, &team_stats::yds_per_play, order);
					cout << "Team\t\t\t" << "Yards Per Play\n";
					sort_by_field(teamData[i].teams, &team_stats::yds_per_play);
				}
				else if (field == "first_per_game") {
					sort_by_field(teamData[i].teams, 32, &team_stats::first_per_game, order);
					cout << "Team\t\t\t" << "First Downs Per Game\n";
					sort_by_field(teamData[i].teams, &team_stats::first_per_game);
				}
				else if (field == "third_md") {
					sort_by_field(teamData[i].teams, 32, &team_stats::third_md, order);
					cout << "Team\t\t\t" << "3rd Down Conversions\n";
					sort_by_field(teamData[i].teams, &team_stats::third_md);
				}
				else if (field == "third_att") {
					sort_by_field(teamData[i].teams, 32, &team_stats::third_att, order);
					cout << "Team\t\t\t" << "3rd Downs Attempted\n";
					sort_by_field(teamData[i].teams, &team_stats::third_att);
				}
				else if (field == "third_pct") {
					sort_by_field(teamData[i].teams, 32, &team_stats::third_pct, order);
					cout << "Team\t\t\t" << "3rd Down Conversion Percentage\n";
					sort_by_field(teamData[i].teams, &team_stats::third_pct);
				}
				else if (field == "fourth_md") {
					sort_by_field(teamData[i].teams, 32, &team_stats::fourth_md, order);
					cout << "Team\t\t\t" << "4th Down Conversions\n";
					sort_by_field(teamData[i].teams, &team_stats::fourth_md);
				}
				else if (field == "fouth_att") {
					sort_by_field(teamData[i].teams, 32, &team_stats::fourth_md, order);
					cout << "Team\t\t\t" << "4th Downs Attempted\n";
					sort_by_field(teamData[i].teams, &team_stats::fourth_md);
				}
				else if (field == "fourt_pct") {
					sort_by_field(teamData[i].teams, 32, &team_stats::fourth_pct, order);
					cout << "Team\t\t\t" << "4th Down Conversion Percentage\n";
					sort_by_field(teamData[i].teams, &team_stats::fourth_pct);
				}
				else if (field == "penalties") {
					sort_by_field(teamData[i].teams, 32, &team_stats::penalties, order);
					cout << "Team\t\t\t" << "Penalties\n";
					sort_by_field(teamData[i].teams, &team_stats::penalties);
				}
				else if (field == "pen_yds") {
					sort_by_field(teamData[i].teams, 32, &team_stats::pen_yds, order);
					cout << "Team\t\t\t" << "Yards Lost From Penalties\n";
					sort_by_field(teamData[i].teams, &team_stats::pen_yds);
				}
				else if (field == "fum") {
					sort_by_field(teamData[i].teams, 32, &team_stats::fum, order);
					cout << "Team\t\t\t" << "Fumbles\n";
					sort_by_field(teamData[i].teams, &team_stats::fum);
				}
				else if (field == "lost") {
					sort_by_field(teamData[i].teams, 32, &team_stats::lost, order);
					cout << "Team\t\t\t" << "Fumbles Lost\n";
					sort_by_field(teamData[i].teams, &team_stats::lost);
				}
				else if (field == "to") {
					sort_by_field(teamData[i].teams, 32, &team_stats::to, order);
					cout << "Team\t\t\t" << "Turnover Ratio\n";
					sort_by_field(teamData[i].teams, &team_stats::to);
				}
				else cout << "Field not recognized.\n";
				cout << "\n";
			}
		}
		else if (sortType == "qfind") { // qfind commands
			cin >> startYear;
			cin >> field;
			cin >> order;
			cout << "\n" << sortType << " " << startYear << " " << field << " " << order << "\n"; // print command

			int i = 0;
			while (teamData[i].year != startYear) i++;

			// Sorts the array in increasing order by the selected field
			if (field == "pts_per_game") sort_by_field(teamData[i].teams, 32, &team_stats::pts_per_game, "incr"); // call to sort
			else if (field == "total_points") sort_by_field(teamData[i].teams, 32, &team_stats::total_points, "incr");
			else if (field == "scrimmage_plays") sort_by_field(teamData[i].teams, 32, &team_stats::scrimmage_plays, "incr");
			else if (field == "yds_per_game") sort_by_field(teamData[i].teams, 32, &team_stats::yds_per_game, "incr");
			else if (field == "yds_per_play") sort_by_field(teamData[i].teams, 32, &team_stats::yds_per_play, "incr");
			else if (field == "first_per_game") sort_by_field(teamData[i].teams, 32, &team_stats::first_per_game, "incr");
			else if (field == "third_md") sort_by_field(teamData[i].teams, 32, &team_stats::third_md, "incr");
			else if (field == "third_att") sort_by_field(teamData[i].teams, 32, &team_stats::third_att, "incr");
			else if (field == "third_pct") sort_by_field(teamData[i].teams, 32, &team_stats::third_pct, "incr");
			else if (field == "fourth_md") sort_by_field(teamData[i].teams, 32, &team_stats::fourth_md, "incr");
			else if (field == "fourth_att") sort_by_field(teamData[i].teams, 32, &team_stats::fourth_md, "incr");
			else if (field == "fourth_pct") sort_by_field(teamData[i].teams, 32, &team_stats::fourth_pct, "incr");
			else if (field == "penalties") sort_by_field(teamData[i].teams, 32, &team_stats::penalties, "incr");
			else if (field == "pen_yds") sort_by_field(teamData[i].teams, 32, &team_stats::pen_yds, "incr");
			else if (field == "fum") sort_by_field(teamData[i].teams, 32, &team_stats::fum, "incr");
			else if (field == "lost") sort_by_field(teamData[i].teams, 32, &team_stats::lost, "incr");
			else if (field == "to") sort_by_field(teamData[i].teams, 32, &team_stats::to, "incr");
			else if (field == "top_per_game") quickSortTOP(teamData, 0, 32, i, "incr");
			else
				cout << "Field not recognized\n";

			if (order != "average") {
				if (order == "min") { // qfind min
					cout << "Team\t\t\t\t" << "Min " << field << "\n";
					cout << "\"" << teamData[i].teams[0].team_name << "\"\t\t";
					if (field == "pts_per_game") cout << teamData[i].teams[0].pts_per_game;
					else if (field == "total_points") cout << teamData[i].teams[0].total_points;
					else if (field == "scrimmage_plays") cout << teamData[i].teams[0].scrimmage_plays;
					else if (field == "yds_per_game") cout << teamData[i].teams[0].yds_per_game;
					else if (field == "yds_per_play") cout << teamData[i].teams[0].yds_per_play;
					else if (field == "first_per_game") cout << teamData[i].teams[0].first_per_game;
					else if (field == "third_md") cout << teamData[i].teams[0].third_md;
					else if (field == "third_att") cout << teamData[i].teams[0].third_att;
					else if (field == "third_pct") cout << teamData[i].teams[0].third_pct;
					else if (field == "fourth_md") cout << teamData[i].teams[0].fourth_md;
					else if (field == "fourth_att") cout << teamData[i].teams[0].fourth_att;
					else if (field == "fourth_pct") cout << teamData[i].teams[0].fourth_pct;
					else if (field == "penalties") cout << teamData[i].teams[0].penalties;
					else if (field == "pen_yds") cout << teamData[i].teams[0].pen_yds;
					else if (field == "fum") cout << teamData[i].teams[0].fum;
					else if (field == "lost") cout << teamData[i].teams[0].lost;
					else if (field == "to") cout << teamData[i].teams[0].to;
					else if (field == "top_per_game") cout << teamData[i].teams[0].top_per_game;
					else
						cout << "Field not recognized\n";
				}
				else if (order == "max") { // qfind max
					cout << "Team\t\t\t\t" << "Max " << field << "\n";
					cout << "\"" << teamData[i].teams[31].team_name << "\"\t\t";
					if (field == "pts_per_game") cout << teamData[i].teams[31].pts_per_game;
					else if (field == "total_points") cout << teamData[i].teams[31].total_points;
					else if (field == "scrimmage_plays") cout << teamData[i].teams[31].scrimmage_plays;
					else if (field == "yds_per_game") cout << teamData[i].teams[31].yds_per_game;
					else if (field == "yds_per_play") cout << teamData[i].teams[31].yds_per_play;
					else if (field == "first_per_game") cout << teamData[i].teams[31].first_per_game;
					else if (field == "third_md") cout << teamData[i].teams[31].third_md;
					else if (field == "third_att") cout << teamData[i].teams[31].third_att;
					else if (field == "third_pct") cout << teamData[i].teams[31].third_pct;
					else if (field == "fourth_md") cout << teamData[i].teams[31].fourth_md;
					else if (field == "fourth_att") cout << teamData[i].teams[31].fourth_att;
					else if (field == "fourth_pct") cout << teamData[i].teams[31].fourth_pct;
					else if (field == "penalties") cout << teamData[i].teams[31].penalties;
					else if (field == "pen_yds") cout << teamData[i].teams[31].pen_yds;
					else if (field == "fum") cout << teamData[i].teams[31].fum;
					else if (field == "lost") cout << teamData[i].teams[31].lost;
					else if (field == "to") cout << teamData[i].teams[31].to;
					else if (field == "top_per_game") cout << teamData[i].teams[31].top_per_game;
					else
						cout << "Field not recognized\n";
				}
				else { // qfind median
					cout << "Median " << field << "\n";
					if (field == "pts_per_game") cout << teamData[i].teams[15].pts_per_game;
					else if (field == "total_points") cout << teamData[i].teams[15].total_points;
					else if (field == "scrimmage_plays") cout << teamData[i].teams[15].scrimmage_plays;
					else if (field == "yds_per_game") cout << teamData[i].teams[15].yds_per_game;
					else if (field == "yds_per_play") cout << teamData[i].teams[15].yds_per_play;
					else if (field == "first_per_game") cout << teamData[i].teams[15].first_per_game;
					else if (field == "third_md") cout << teamData[i].teams[15].third_md;
					else if (field == "third_att") cout << teamData[i].teams[15].third_att;
					else if (field == "third_pct") cout << teamData[i].teams[15].third_pct;
					else if (field == "fourth_md") cout << teamData[i].teams[15].fourth_md;
					else if (field == "fourth_att") cout << teamData[i].teams[15].fourth_att;
					else if (field == "fourth_pct") cout << teamData[i].teams[15].fourth_pct;
					else if (field == "penalties") cout << teamData[i].teams[15].penalties;
					else if (field == "pen_yds") cout << teamData[i].teams[15].pen_yds;
					else if (field == "fum") cout << teamData[i].teams[15].fum;
					else if (field == "lost") cout << teamData[i].teams[15].lost;
					else if (field == "to") cout << teamData[i].teams[15].to;
					else if (field == "top_per_game") cout << teamData[i].teams[15].top_per_game;
					else
						cout << "Field not recognized\n";
				}
			}
			else if (order == "average") { // qfind average
				cout << "Average " << field << "\n";
				if (field == "pts_per_game") cout << compute_average(teamData[i].teams, &team_stats::first_per_game);
				else if (field == "total_points") cout << compute_average(teamData[i].teams, &team_stats::total_points);
				else if (field == "scrimmage_plays") cout << compute_average(teamData[i].teams, &team_stats::scrimmage_plays);
				else if (field == "yds_per_game") cout << compute_average(teamData[i].teams, &team_stats::yds_per_game);
				else if (field == "yds_per_play") cout << compute_average(teamData[i].teams, &team_stats::yds_per_play);
				else if (field == "first_per_game") cout << compute_average(teamData[i].teams, &team_stats::first_per_game);
				else if (field == "third_md") cout << compute_average(teamData[i].teams, &team_stats::third_md);
				else if (field == "third_att") cout << compute_average(teamData[i].teams, &team_stats::third_att);
				else if (field == "third_pct") cout << compute_average(teamData[i].teams, &team_stats::third_pct);
				else if (field == "fourth_md") cout << compute_average(teamData[i].teams, &team_stats::fourth_md);
				else if (field == "fourth_att") cout << compute_average(teamData[i].teams, &team_stats::fourth_att);
				else if (field == "fourth_pct") cout << compute_average(teamData[i].teams, &team_stats::fourth_pct);
				else if (field == "penalties") cout << compute_average(teamData[i].teams, &team_stats::penalties);
				else if (field == "pen_yds") cout << compute_average(teamData[i].teams, &team_stats::pen_yds);
				else if (field == "fum") cout << compute_average(teamData[i].teams, &team_stats::fum);
				else if (field == "lost") cout << compute_average(teamData[i].teams, &team_stats::lost);
				else if (field == "to") cout << compute_average(teamData[i].teams, &team_stats::to);
				else
					cout << "Field not recognized\n";
			}
			else
				cout << "Order not recognized.\n";
			cout << "\n";
		}
		else
			cout << "Command " << sortType << " not found.\n";
	}

	delete teamData;
	return 0;
}
/*
 * END OF MAIN
 */
 
 // Bubblesort method used with the bsort range command.
void bSortRange(annual_stats *arr, int startYear, int endYear, string field, string order, int numYears) {
	bool swapped = true;
	int k = 0, index = 0;

	int i = 0, j = 0;
	while (arr[i].year != startYear) i++;
	while (arr[j].year != endYear) j++;

	int totalNumStats = (j - i + 1) * 32;

	team_stats* allYears = new team_stats[totalNumStats];

	// collection of all data across given range for sorting
	int m = 0;
	for (int k = i; k <= j; k++) {
		for (int l = 0; l < 32; l++) {
			allYears[m] = arr[k].teams[l];
			allYears[m].year = arr[k].year;
			m++;
		}
	}

	cout << "Team\t\t\t" << field << "\tYear\n";

	startYear = endYear;

	// Long chain if-else used to sort different fields
	// POINTS PER GAME
	if (field == "pts_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].pts_per_game, allYears[j + 1].pts_per_game, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::pts_per_game);
	}
	// TOTAL POINTS
	else if (field == "total_points") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].total_points, allYears[j + 1].total_points, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::total_points);
	}
	// SCRIMMAGE PLAYS
	else if (field == "scrimmage_plays") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].scrimmage_plays, allYears[j + 1].scrimmage_plays, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::scrimmage_plays);
	}
	// YARDS PER GAME
	else if (field == "yds_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].yds_per_game, allYears[j + 1].yds_per_game, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::yds_per_game);
	}
	// YARDS PER PLAY
	else if (field == "yds_per_play") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].yds_per_play, allYears[j + 1].yds_per_play, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::yds_per_play);
	}
	// FIRST DOWNS PER GAME
	else if (field == "first_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].first_per_game, allYears[j + 1].first_per_game, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::first_per_game);
	}
	// THIRD DOWNS MADE
	else if (field == "third_md") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].third_md, allYears[j + 1].third_md, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::third_md);
	}
	// THIRD DOWNS ATTEMPTED
	else if (field == "third_att") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].third_att, allYears[j + 1].third_att, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::third_att);
	}
	// PERCENT OF THIRD DOWN CONVERSIONS
	else if (field == "third_pct") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].third_pct, allYears[j + 1].third_pct, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::third_pct);
	}
	// FOURTH DOWNS MADE
	else if (field == "fourth_md") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].fourth_md, allYears[j + 1].fourth_md, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::fourth_md);
	}
	// FOURTH DOWNS ATTEMPTED
	else if (field == "fourth_att") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].fourth_att, allYears[j + 1].fourth_att, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::fourth_att);
	}
	// PERCENT OF THIRD DOWN CONVERSIONS
	else if (field == "fourth_pct") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].fourth_pct, allYears[j + 1].fourth_pct, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::fourth_pct);
	}
	// PENALTIES
	else if (field == "penalties") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].penalties, allYears[j + 1].penalties, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::penalties);
	}
	// PENALTY YARDS
	else if (field == "pen_yds") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].pen_yds, allYears[j + 1].pen_yds, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::pen_yds);
	}
	// TIME OF POSSESSION
	else if (field == "top_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].top_per_game, allYears[j + 1].top_per_game, order);
			}
		}
	}
	// FUMBLES
	else if (field == "fum") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].fum, allYears[j + 1].fum, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::fum);
	}
	// FUMBLES LOST
	else if (field == "lost") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].lost, allYears[j + 1].lost, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::lost);
	}
	// TURNOVER RATIO
	else if (field == "to") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < totalNumStats - k; j++) {
				swapped = bSortSwap(allYears, j, allYears[j].to, allYears[j + 1].to, order);
			}
		}
		range_print(allYears, totalNumStats, &team_stats::to);
	}
	else
		cout << "Field " << field << " not found.\n";
	delete allYears;
}

// Bubblesort method used with the bsort command.
void bSortYear(annual_stats *arr, int startYear, string field, string order, int numYears) {
	bool swapped = true;
	int k = 0, index;

	// Get the index of the year
	for (int i = 0; i < numYears; i++)
		if (arr[i].year == startYear)
			index = i;

	if (field == "team_name")
		cout << "Team\n";
	else
		cout << "Team\t\t\t" << field << "\n";

	// Long chain if-else used to sort different fields
	// TEAM NAME
	if (field == "team_name") {
		for (int z = 0; z < 3; z++) {
			swapped = true;
			k = 0;
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, string(arr[index].teams[j].team_name), string(arr[index].teams[j + 1].team_name), order);
				}
			}
		}
		for (int i = 0; i < 32; i++) {
			cout << "\"" << arr[index].teams[i].team_name << "\"\n";
		}
	}
	// POINTS PER GAME
	else if (field == "pts_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].pts_per_game, arr[index].teams[j + 1].pts_per_game, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].pts_per_game << "\n";
		}
	}
	// TOTAL POINTS
	else if (field == "total_points") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].total_points, arr[index].teams[j + 1].total_points, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].total_points << "\n";
		}
	}
	// SCRIMMAGE PLAYS
	else if (field == "scrimmage_plays") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].scrimmage_plays, arr[index].teams[j + 1].scrimmage_plays, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].scrimmage_plays << "\n";
		}
	}
	// YARDS PER GAME
	else if (field == "yds_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].yds_per_game, arr[index].teams[j + 1].yds_per_game, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].yds_per_game << "\n";
		}
	}
	// YARDS PER PLAY
	else if (field == "yds_per_play") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].yds_per_play, arr[index].teams[j + 1].yds_per_play, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].yds_per_play << "\n";
		}
	}
	// FIRST DOWNS PER GAME
	else if (field == "first_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].first_per_game, arr[index].teams[j + 1].first_per_game, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].first_per_game << "\n";
		}
	}
	// THIRD DOWNS MADE
	else if (field == "third_md") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_md, arr[index].teams[j + 1].third_md, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].third_md << "\n";
		}
	}
	// THIRD DOWNS ATTEMPTED
	else if (field == "third_att") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_att, arr[index].teams[j + 1].third_att, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].third_att << "\n";
		}
	}
	// PERCENT OF THIRD DOWN CONVERSIONS
	else if (field == "third_pct") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_pct, arr[index].teams[j + 1].third_pct, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].third_pct << "\n";
		}
	}
	// FOURTH DOWNS MADE
	else if (field == "fourth_md") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_md, arr[index].teams[j + 1].fourth_md, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].fourth_md << "\n";
		}
	}
	// FOURTH DOWNS ATTEMPTED
	else if (field == "fourth_att") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_att, arr[index].teams[j + 1].fourth_att, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].fourth_att << "\n";
		}
	}
	// PERCENTAGE OF FOURTH DOWN CONVERSIONS
	else if (field == "fourth_pct") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_pct, arr[index].teams[j + 1].fourth_pct, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].fourth_pct << "\n";
		}
	}
	// PENALTIES
	else if (field == "penalties") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].penalties, arr[index].teams[j + 1].penalties, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].penalties << "\n";
		}
	}
	// PENALTY YARDS
	else if (field == "pen_yds") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].pen_yds, arr[index].teams[j + 1].pen_yds, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].pen_yds << "\n";
		}
	}
	// TIME OF POSSESSION
	else if (field == "top_per_game") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].top_per_game, arr[index].teams[j + 1].top_per_game, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].top_per_game << "\n";
		}
	}
	// FUMBLES
	else if (field == "fum") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].fum, arr[index].teams[j + 1].fum, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].fum << "\n";
		}
	}
	// FUMBLES LOST
	else if (field == "lost") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].lost, arr[index].teams[j + 1].lost, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].lost << "\n";
		}
	}
	// TURNOVER RATIO
	else if (field == "to") {
		while (swapped) {
			swapped = false;
			k++;
			for (int j = 0; j < 32 - k; j++) {
				swapped = bSortSwap(arr, index, j, arr[index].teams[j].to, arr[index].teams[j + 1].to, order);
			}
		}
		for (int i = 0; i < 32; i++) {
			string str = arr[index].teams[i].team_name;
			cout << "\"" << str << "\"\t";
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << arr[index].teams[i].to << "\n";
		}
	}
	else
		cout << "Field " << field << " not found.\n";
}

// Bubblesort method used with the bfind command.
void bFind(annual_stats *arr, int startYear, string field, string order, int numYears) {
	bool swapped = true;
	int k = 0, index;
	string sortOrder;
	
	for (int i = 0; i < numYears; i++)
		if (arr[i].year == startYear)
			index = i;

	// Long chain if-else used to sort different fields
	if (order != "average") { // average does not need to be sorted
		if (order == "min") sortOrder = "incr";
		if (order == "max" || order == "median") sortOrder = "decr";
		// POINTS PER GAME
		if (field == "pts_per_game") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].pts_per_game, arr[index].teams[j + 1].pts_per_game, sortOrder);
				}
			}
		}
		// TOTAL POINTS
		else if (field == "total_points") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].total_points, arr[index].teams[j + 1].total_points, sortOrder);
				}
			}
		}
		// SCRIMMAGE PLAYS
		else if (field == "scrimmage_plays") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].scrimmage_plays, arr[index].teams[j + 1].scrimmage_plays, sortOrder);
				}
			}
		}
		// YARDS PER GAME
		else if (field == "yds_per_game") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].yds_per_game, arr[index].teams[j + 1].yds_per_game, sortOrder);
				}
			}
		}
		// YARDS PER PLAY
		else if (field == "yds_per_play") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].yds_per_play, arr[index].teams[j + 1].yds_per_play, sortOrder);
				}
			}
		}
		// FIRST DOWNS PER GAME
		else if (field == "first_per_game") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].first_per_game, arr[index].teams[j + 1].first_per_game, sortOrder);
				}
			}
		}
		// THIRD DOWNS MADE
		else if (field == "third_md") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_md, arr[index].teams[j + 1].third_md, sortOrder);
				}
			}
		}
		// THIRD DOWNS ATTEMPTED
		else if (field == "third_att") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_att, arr[index].teams[j + 1].third_att, sortOrder);
				}
			}
		}
		// PERCENTAGE OF THIRD DOWN CONVERSIONS
		else if (field == "third_pct") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].third_pct, arr[index].teams[j + 1].third_pct, sortOrder);
				}
			}
		}
		// FOURTH DOWNS MADE
		else if (field == "fourth_md") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_md, arr[index].teams[j + 1].fourth_md, sortOrder);
				}
			}
		}
		// FOURTH DOWNS ATTEMPTED
		else if (field == "fourth_att") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_att, arr[index].teams[j + 1].fourth_att, sortOrder);
				}
			}
		}
		// PERCENTAGE OF FOURTH DOWN CONVERSIONS
		else if (field == "fourth_pct") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].fourth_pct, arr[index].teams[j + 1].fourth_pct, sortOrder);
				}
			}
		}
		// PENALTIES
		else if (field == "penalties") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].penalties, arr[index].teams[j + 1].penalties, sortOrder);
				}
			}
		}
		// PENALTY YARDS
		else if (field == "pen_yds") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].pen_yds, arr[index].teams[j + 1].pen_yds, sortOrder);
				}
			}
		}
		// TIME OF POSSESSION
		else if (field == "top_per_game") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].top_per_game, arr[index].teams[j + 1].top_per_game, sortOrder);
				}
			}
		}
		// FUMBLES
		else if (field == "fum") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].fum, arr[index].teams[j + 1].fum, sortOrder);
				}
			}
		}
		// FUMBLES LOST
		else if (field == "lost") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].lost, arr[index].teams[j + 1].lost, sortOrder);
				}
			}
		}
		// TURNOVER RATIO
		else if (field == "to") {
			while (swapped) {
				swapped = false;
				k++;
				for (int j = 0; j < 32 - k; j++) {
					swapped = bSortSwap(arr, index, j, arr[index].teams[j].to, arr[index].teams[j + 1].to, sortOrder);
				}
			}
		}
		else
			cout << "Field " << field << " not found.\n";
	}

	bFindPrint(arr, field, order, index); // call to print data
}

// Called from inside bFind() for printing information to the console.
void bFindPrint(annual_stats *arr, string field, string order, int index) {
	// Long chain if-else used to sort different fields
	// POINTS PER GAME
	if (field == "pts_per_game") {
		if (order == "max" || order == "min") {
			float key = arr[index].teams[0].pts_per_game;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].pts_per_game == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].pts_per_game << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].pts_per_game;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].pts_per_game << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// TOTAL POINTS
	else if (field == "total_points") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].total_points;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].total_points == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].total_points << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].total_points;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].total_points << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// SCRIMMAGE PLAYS
	else if (field == "scrimmage_plays") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].scrimmage_plays;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].scrimmage_plays == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].scrimmage_plays << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].scrimmage_plays;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].scrimmage_plays << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// YARDS PER GAME
	else if (field == "yds_per_game") {
		if (order == "max" || order == "min") {
			float key = arr[index].teams[0].yds_per_game;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].yds_per_game == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].yds_per_game << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].yds_per_game;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].yds_per_game << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// YARDS PER PLAY
	else if (field == "yds_per_play") {
		if (order == "max" || order == "min") {
			float key = arr[index].teams[0].yds_per_play;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].yds_per_play == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].yds_per_play << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].yds_per_play;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].yds_per_play << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// FIRST DOWNS PER GAME
	else if (field == "first_per_game") {
		if (order == "max" || order == "min") {
			float key = arr[index].teams[0].first_per_game;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].first_per_game == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].first_per_game << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].first_per_game;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].first_per_game << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// THIRD DOWNS MADE
	else if (field == "third_md") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].third_md;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].third_md == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].third_md << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].third_md;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].third_md << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// THIRD DOWNS ATTEMPTED
	else if (field == "third_att") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].third_att;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].third_att == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].third_att << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].third_att;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].third_att << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// PERCENTAGE OF THIRD DOWN CONVERSIONS
	else if (field == "third_pct") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].third_pct;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].third_pct == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].third_pct << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].third_pct;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].third_pct << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// FOURTH DOWNS MADE
	else if (field == "fourth_md") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].fourth_md;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].fourth_md == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].fourth_md << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].fourth_md;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].fourth_md << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// FOURTH DOWNS ATTEMPTED
	else if (field == "fourth_att") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].fourth_att;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].fourth_att == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].fourth_att << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].fourth_att;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].fourth_att << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// PERCENTAGE OF FOURTH DOWN CONVERSIONS
	else if (field == "fourth_pct") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].fourth_pct;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].fourth_pct == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].fourth_pct << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].fourth_pct;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].fourth_pct << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// PENALTIES
	else if (field == "penalties") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].penalties;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].penalties == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].penalties << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].penalties;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].penalties << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// PENALTY YARDS
	else if (field == "pen_yds") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].pen_yds;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].pen_yds == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].pen_yds << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].pen_yds;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].pen_yds << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// FUMBLES
	else if (field == "fum") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].fum;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].fum == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].fum << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].fum;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].fum << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// FUMBLES LOST
	else if (field == "lost") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].lost;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].lost == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].lost << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].lost;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].lost << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	// TURNOVER RATIO
	else if (field == "to") {
		if (order == "max" || order == "min") {
			int key = arr[index].teams[0].to;
			int count = 1;

			cout << "Team\t\t\t" << order << " " << field << "\n";

			for (int i = 1; i < 32; i++)
				if (arr[index].teams[i].to == key)
					count++;

			for (int i = 0; i < count; i++) {
				string str = arr[index].teams[i].team_name;
				cout << "\"" << str << "\"\t";
				if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
					cout << "\t"; // extra tab for teams with extra short names
				cout << arr[index].teams[i].to << "\n";
			}
		}
		else if (order == "average") {
			float average = 0;
			for (int i = 0; i < 32; i++)
				average += arr[index].teams[i].to;

			average /= 32;

			cout << order << " " << field << "\n";
			cout << average << "\n";
		}
		else if (order == "median") {
			cout << order << " " << field << "\n";
			cout << arr[index].teams[15].to << "\n";
		}
		else {
			cout << "Item " << order << " not recognized.";
		}
	}
	else
		cout << "Field " << field << " not found.\n";
}

#pragma warning ( disable :  4715 )
// Swaps data in the array of structs using Bubblesort method.
bool bSortSwap(annual_stats *arr, int index, int j, string str1, string str2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (str1.compare(str2) < 0) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (str1.compare(str2) >= 0) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else
		cout << "Order " << order << " not recognized.";
}

// Swaps data in the array of structs using Bubblesort method.
bool bSortSwap(annual_stats *arr, int index, int j, int num1, int num2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (num1 < num2) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (num1 > num2) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else {
		cout << "Order " << order << " not recognized.";
	}
}

// Swaps data in the array of structs using Bubblesort method.
bool bSortSwap(annual_stats *arr, int index, int j, float num1, float num2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (num1 < num2) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (num1 > num2) {
			tmp1 = arr[index].teams[j];
			tmp2 = &arr[index].teams[j];
			tmp3 = &arr[index].teams[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else
		cout << "Order " << order << " not recognized.";
}

// Swaps data in the array of structs using Bubblesort method. For bsort range
bool bSortSwap(team_stats *arr, int j, string str1, string str2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (str1.compare(str2) < 0) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (str1.compare(str2) >= 0) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else
		cout << "Order " << order << " not recognized.";
}

// Swaps data in the array of structs using Bubblesort method. For bsort range
bool bSortSwap(team_stats *arr, int j, int num1, int num2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (num1 < num2) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (num1 > num2) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else
		cout << "Order " << order << " not recognized.";
}

// Swaps data in the array of structs using Bubblesort method. For bsort range
bool bSortSwap(team_stats *arr, int j, float num1, float num2, string order) {
	team_stats tmp1, *tmp2, *tmp3;

	if (order == "decr") {
		if (num1 < num2) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else if (order == "incr") {
		if (num1 > num2) {
			tmp1 = arr[j];
			tmp2 = &arr[j];
			tmp3 = &arr[j + 1];
			*tmp2 = *tmp3;
			*tmp3 = tmp1;
			return true;
		}
	}
	else
		cout << "Order " << order << " not recognized.";
}

// Primary quicksort sorting algorithm.
template <typename T, typename M> void quickSort(team_stats *arr, int top, int bottom, const M T::*p, string order) {
	int middle;
	if (top < bottom)
	{
		middle = partition(arr, top, bottom, p, order);
		quickSort(arr, top, middle, p, order);   // sort first section
		quickSort(arr, middle + 1, bottom, p, order);    // sort second section
	}
}
#pragma warning ( disable: 4172 )
// Partition for quicksort
template <typename T, typename M> int partition(team_stats *arr, int top, int bottom, const M T::*p, string order) {
	int i = top - 1;
	int j = bottom + 1;
	team_stats temp;

	if (order == "decr") {
		do {
			do {
				j--;
			} while (arr[top].*p > arr[j].*p);

			do {
				i++;
			} while (arr[top].*p < arr[i].*p);

			if (i < j) {
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}

		} while (i < j);
		return j;
	}
	else {
		do {
			do {
				j--;
			} while (arr[top].*p < arr[j].*p);

			do {
				i++;
			} while (arr[top].*p > arr[i].*p);

			if (i < j) {
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		} while (i < j);
	}
	return j;
}

// Prints the results of qsort year.
template <typename T, typename M> void quickSortPrint(team_stats *arr, const M T::*p) {
	for (int j = 0; j < 32; j++) {
		string str = string(arr[j].team_name);
		cout << "\"" << str << "\"\t";
		if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
			cout << "\t"; // extra tab for teams with extra short names
		cout << arr[j].*p << "\n";
	}
}

// Prints the results of qsort range.
template <typename T, typename M> void quickSortRangePrint(team_stats *arr, const M T::*p, int total) {
	for (int j = 0; j < total; j++) {
		string str = string(arr[j].team_name);
		cout << "\"" << str << "\"\t";
		if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
			cout << "\t"; // extra tab for teams with extra short names
		cout << arr[j].*p << "\t\t" << arr[j].year << "\n";
	}
}

// Quicksorts the array by team name.
void quickSortName(annual_stats *arr, int top, int bottom, int key, string order) {
	int middle;
	if (top < bottom)
	{
		middle = quickSortNamePartition(arr, top, bottom, key, order);
		quickSortName(arr, top, middle, key, order);   // sort first section
		quickSortName(arr, middle + 1, bottom, key, order);    // sort second section
	}
}

// Partition for quicksort array by team name.
int quickSortNamePartition(annual_stats *arr, int top, int bottom, int key, string order) {
	int i = top - 1;
	int j = bottom + 1;
	team_stats temp;
	
	if (order == "decr") {
		do {
			do {
				j--;
			} while (string(arr[key].teams[top].team_name) > string(arr[key].teams[j].team_name));

			do {
				i++;
			} while (string(arr[key].teams[top].team_name) < string(arr[key].teams[i].team_name));

			if (i < j) {
				temp = arr[key].teams[i];
				arr[key].teams[i] = arr[key].teams[j];
				arr[key].teams[j] = temp;
			}

		} while (i < j);
		return j;
	}
	else {
		do {
			do {
				j--;
			} while (string(arr[key].teams[top].team_name) < string(arr[key].teams[j].team_name));

			do {
				i++;
			} while (string(arr[key].teams[top].team_name) > string(arr[key].teams[i].team_name));

			if (i < j) {
				temp = arr[key].teams[i];
				arr[key].teams[i] = arr[key].teams[j];
				arr[key].teams[j] = temp;
			}
		} while (i < j);
	}
	return j;
}

// Quicksorts the array by top_per_game.
void quickSortTOP(annual_stats *arr, int left, int right, int key, string order) {
	int i = left, j = right;
	int pivot = (left + right) / 2;
	team_stats tmp1, *tmp2, *tmp3;

	// partition 
	if (order == "incr") {
		while (i <= j) {
			while (string(arr[key].teams[i].top_per_game) < string(arr[key].teams[pivot].top_per_game)) {
				i++;
			}
			while (string(arr[key].teams[j].top_per_game) > string(arr[key].teams[pivot].top_per_game)) {
				j--;
			}
			if (i <= j) {
				tmp1 = arr[key].teams[i];
				tmp2 = &arr[key].teams[i];
				tmp3 = &arr[key].teams[j];
				*tmp2 = *tmp3;
				*tmp3 = tmp1;
				i++;
				j--;
			}
		}
	}
	else if (order == "decr") {
		while (i <= j) {
			while (string(arr[key].teams[i].top_per_game) > string(arr[key].teams[pivot].top_per_game)) {
				i++;
			}
			while (string(arr[key].teams[j].top_per_game) > string(arr[key].teams[pivot].top_per_game)) {
				j--;
			}
			if (i <= j) {
				tmp1 = arr[key].teams[i];
				tmp2 = &arr[key].teams[i];
				tmp3 = &arr[key].teams[j];
				*tmp2 = *tmp3;
				*tmp3 = tmp1;
				i++;
				j--;
			}
		}
	}
	else
		cout << "Order not recognized.\n";

	// recursion 
	if (left < j)
		quickSortTOP(arr, left, j, key, order);
	if (i < right)
		quickSortTOP(arr, i, right, key, order);
}

// Computes the average of a field across whole array.
template <typename T, typename M> float computeAverage(team_stats *arr, const M T::*p) {
	float total = 0;
	for (int i = 0; i < 32; i++) total += arr[i].*p;
	return (total / 32);
}

// Prints the whole array.
void printArray(annual_stats* teamData, int numYears) {
	cout << numYears << "\n";
	for (int i = 0; i < numYears; i++) {
		cout << teamData[i].year << "\n";
		for (int j = 0; j < 32; j++) {
			cout << "\"" << teamData[i].teams[j].team_name << "\"\t";
			string str = teamData[i].teams[j].team_name;
			if (str == "New York Jets" || str == "Buffalo Bills" || str == "Detroit Lions" || str == "Chicago Bears")
				cout << "\t"; // extra tab for teams with extra short names
			cout << teamData[i].teams[j].games << "\t";
			cout << teamData[i].teams[j].pts_per_game << "\t";
			cout << teamData[i].teams[j].total_points << "\t";
			cout << teamData[i].teams[j].scrimmage_plays << "\t";
			cout << teamData[i].teams[j].yds_per_game << "\t";
			cout << teamData[i].teams[j].yds_per_play << "\t";
			cout << teamData[i].teams[j].first_per_game << "\t";
			cout << teamData[i].teams[j].third_md << "\t";
			cout << teamData[i].teams[j].third_att << "\t";
			cout << teamData[i].teams[j].third_pct << "\t";
			cout << teamData[i].teams[j].fourth_md << "\t";
			cout << teamData[i].teams[j].fourth_att << "\t";
			cout << teamData[i].teams[j].fourth_pct << "\t";
			cout << teamData[i].teams[j].penalties << "\t";
			cout << teamData[i].teams[j].pen_yds << "\t";
			cout << "\"" << teamData[i].teams[j].top_per_game << "\"\t";
			cout << teamData[i].teams[j].fum << "\t";
			cout << teamData[i].teams[j].lost << "\t";
			if (teamData[i].teams[j].to > 0) cout << "+";
			cout << teamData[i].teams[j].to << "\n";
		}
	}
}