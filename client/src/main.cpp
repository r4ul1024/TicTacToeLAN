#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>

boost::asio::io_context io;
boost::asio::ip::tcp::socket serverSocket(io);
char data[1];

bool game = true;
std::vector<char> area(9);
std::string IP;
char symbol[1];

void logo_display();
void connect();
void area_display();
void move();
void restart();

int main() {
	connect();
	area_display();

	while (game) {
		boost::asio::read(serverSocket, boost::asio::buffer(symbol));
		if (symbol[0] == 'W') {
			std::cout << "YOU WIN!!!\n\n";
			restart();
		} else if (symbol[0] == 'L') {
			std::cout << "YOU LOSE!!!\n\n";
			restart();
		} else if (symbol[0] == 'D') {
			std::cout << "DRAW!!!\n\n";
			restart();
		}
		else if (symbol[0] == 'X') {
			std::cout << "Move: ";
			move();
			std::cout << "\033[2J\033[H";
			area_display();
		} else {
			try {
				std::cout << "Wait...\n";
				std::cout << "\033[2J\033[H";
				area_display();
			} catch (std::exception &e) {
				std::cout << "\033[31mError: " << e.what() << "\033[0m\n";
			}
		}
	}

	return 0;
}

void logo_display() {
	std::cout << R"(
████████ ████████ ████████    ██       █████  ███    ██ 
   ██       ██       ██       ██      ██   ██ ████   ██ 
   ██       ██       ██ █████ ██      ███████ ██ ██  ██ 
   ██       ██       ██       ██      ██   ██ ██  ██ ██ 
   ██       ██       ██       ███████ ██   ██ ██   ████ 
   )";

	std::cout << "\n\n";
}

void connect() {
	while (true) {
		try {
			logo_display();
			std::cout << "Enter IP: ";
			std::cin >> IP;
			serverSocket.connect(boost::asio::ip::tcp::endpoint(
				boost::asio::ip::make_address(IP), 1234));
			std::cout << "\033[32mSuccessful connection\033[0m\n\n";
			break;
		} catch (std::exception &e) {
			std::cout << "\033[2J\033[H";
			std::cout << "\033[31mConnection error: " << e.what()
					  << "\033[0m\n";
		}
	}
}

void area_display() {
	boost::asio::read(serverSocket, boost::asio::buffer(area));
	std::cout << "|" << area[0] << "|" << area[1] << "|" << area[2] << "|\n";
	std::cout << "|" << area[3] << "|" << area[4] << "|" << area[5] << "|\n";
	std::cout << "|" << area[6] << "|" << area[7] << "|" << area[8] << "|\n";
}

void move() {
	bool flag = true;
	while (flag) {
		std::cin >> data;
		for (int i = 0; i < area.size(); i++) {
			if (data[0] == area[i] && data[0] != 'X' && data[0] != 'O') {
				boost::asio::write(serverSocket, boost::asio::buffer(data));
				flag = false;
				break;
			}
		}
	}
}

void restart() {
	char choice;
	std::cout << "[1]-Restart\n";
	std::cout << "[ALL]-Exit\n\n";
	std::cout << "Enter: ";
	std::cin >> choice;

	if (choice == '1') {
		data[0] = 'R';
	} else {
		data[0] = 'E';
		game = false;
	}
	boost::asio::write(serverSocket, boost::asio::buffer(data));
}
