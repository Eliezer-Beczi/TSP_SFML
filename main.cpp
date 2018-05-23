// Beczi Eliezer
// beim1697
// 511

// Grafikus konyvtar: SFML
// Kompilalas: g++ main.cpp -O3 -lsfml-graphics -lsfml-window -lsfml-system
// Futtatas: ./a.out

// Zoom in: I
// Zoom out: O
// Move view: W, A, S, D

#include <cmath>
#include <vector>
#include <limits>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::string;
using std::ifstream;
using std::make_pair;
using std::unordered_map;
using std::numeric_limits;

#define K 16
#define FPS 1
#define OFFSET 2
#define OPTIMIZE 1
#define SHOW_GREEDY 0
#define IMPROVEMENT_LIMIT 2
#define INPUT_FILE "tsp1.txt"

typedef pair<double, vector<int>> tour;
typedef unordered_map<int, pair<int, int>> city;
typedef unordered_map<int, sf::Color> cityColor;

tour myTour;
city cities;
cityColor colors;

sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "TSP", sf::Style::Fullscreen);
sf::View view = window.getDefaultView();

void beOlvas(const string& fileName = INPUT_FILE) {
	ifstream inputFile(fileName);

	int id;
	int x;
	int y;

	while(inputFile >> id && inputFile >> x && inputFile >> y) {
		cities[id] = make_pair(x, y);
		colors[id] = sf::Color(rand() % 255, rand() % 255, rand() % 255);
	}
}

inline void kiIr() {
	cout << myTour.first << endl;

	for(const auto& v : myTour.second) {
		cout << v << ' ';
	}

	cout << endl;
}

inline int dist(const int& a, const int& b) {
	return (pow(cities[b].first - cities[a].first, 2) + pow(cities[b].second - cities[a].second, 2));
}

inline void getTourLength(tour& myTour) {
	int hossz = (myTour.second).size();
	myTour.first = static_cast<double>(0);

	for(int i = 0; i < hossz - 1; ++i) {
		myTour.first += sqrt(dist((myTour.second)[i], (myTour.second)[i + 1]));
	}
}

tour twoOptSwap(const int& i, const int& k) {
	tour newTour;
	int tourSize = (myTour.second).size();

	for(int a = 0; a < i; ++a) {
		(newTour.second).push_back((myTour.second)[a]);
	}

	for(int a = k; a >= i; --a) {
		(newTour.second).push_back((myTour.second)[a]);
	}

	for(int a = k + 1; a < tourSize; ++a) {
		(newTour.second).push_back((myTour.second)[a]);
	}

	getTourLength(newTour);

	return newTour;
}

void showCities() {
	for(const auto& m : cities) {
		sf::CircleShape circle;
		circle.setRadius(7);
		circle.setPointCount(100);
		circle.setFillColor(colors[m.first]);
		circle.setPosition(m.second.first * K, m.second.second * K);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		window.draw(circle);
	}
}

void showAllEdges() {
	int numOfCities = cities.size();

	for(int i = 0; i < numOfCities - 1; ++i) {
		for(int j = i + 1; j < numOfCities; ++j) {
			int x1 = cities[i].first * K;
			int y1 = cities[i].second * K;

			int x2 = cities[j].first * K;
			int y2 = cities[j].second * K;

			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f(x1, y1), colors[i]),
				sf::Vertex(sf::Vector2f(x2, y2), colors[i + 1])
			};

			window.draw(line, 2, sf::Lines);
		}
	}
}

void showEdges(const tour& myTour) {
	int numOfCities = (myTour.second).size() - 1;

	for(int i = 0; i < numOfCities; ++i) {
		int x1 = (cities[(myTour.second)[i]]).first * K;
		int y1 = (cities[(myTour.second)[i]]).second * K;

		int x2 = (cities[(myTour.second)[i + 1]]).first * K;
		int y2 = (cities[(myTour.second)[i + 1]]).second * K;

		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(x1, y1), colors[i]),
			sf::Vertex(sf::Vector2f(x2, y2), colors[i + 1])
		};

		window.draw(line, 2, sf::Lines);
	}
}

void draw(const tour& myTour, const int& numOfIterations) {
	int kontor = 0;

	while(kontor < numOfIterations) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
			view.zoom(0.99f);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
			view.zoom(1.003f);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			view.move(-OFFSET, 0);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			view.move(OFFSET, 0);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			view.move(0, -OFFSET);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			view.move(0, OFFSET);
		}

		window.setView(view);
		window.clear();

		showCities();
		showEdges(myTour);
		// showAllEdges();

		window.display();
		++kontor;
	}
}

tour nearestNeighbour(const int& start) {
	int numOfCities = cities.size();
	bool visited[numOfCities + 1]{false};

	tour newTour;
	(newTour.second).push_back(start);

	int kontor = 1;
	visited[start] = true;

	while(kontor < numOfCities) {
		int index = -1;
		int x = (newTour.second).back();
		int edge = numeric_limits<int>::max();

		for(int y = 1; y <= numOfCities; ++y) {
			if(visited[y]) {
				continue;
			}

			int aux = dist(x, y);

			if(aux < edge) {
				index = y;
				edge = aux;
			}
		}

		(newTour.second).push_back(index);
		visited[index] = true;
		++kontor;

		if(SHOW_GREEDY) {
			draw(newTour, FPS);
		}
	}

	(newTour.second).push_back(start);
	getTourLength(newTour);

	return newTour;
}

void run() {
	int numOfCities = cities.size();
	myTour = nearestNeighbour(1);

	for(int i = 2; i < numOfCities; ++i) {
		tour tmp = nearestNeighbour(i);

		if(tmp.first < myTour.first) {
			myTour = tmp;
		}
	}
}

void twoOpt() {
	int improvement = 0;
	int tourSize = cities.size();

	do {
		++improvement;

		// cout << myTour.first << endl;

		for(int i = 1; i < tourSize - 1; ++i) {
			for(int k = i + 1; k < tourSize; ++k) {
				if(OPTIMIZE) {
					int aux1 = dist((myTour.second)[i - 1], (myTour.second)[i]) + dist((myTour.second)[k], (myTour.second)[k + 1]);
					int aux2 = dist((myTour.second)[i - 1], (myTour.second)[k]) + dist((myTour.second)[i], (myTour.second)[k + 1]);

					if(aux1 <= aux2) {
						continue;
					}
				}

				draw(myTour, FPS);
				tour newTour = twoOptSwap(i, k);

				if(newTour.first < myTour.first) {
					myTour = newTour;
					improvement = 0;
				}
			}
		}
	} while(improvement < IMPROVEMENT_LIMIT);
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	beOlvas();
	run();
	twoOpt();
	kiIr();

	while(window.isOpen()) {
		draw(myTour, 1);
	}

	return 0;
}
