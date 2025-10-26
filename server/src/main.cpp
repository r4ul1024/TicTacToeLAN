#include <boost/asio.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <iostream>
#include <vector>

boost::asio::io_context io;
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1234);
boost::asio::ip::tcp::acceptor acceptor(io, endpoint);
boost::asio::ip::tcp::socket client1Socket(io);
boost::asio::ip::tcp::socket client2Socket(io);
char data[1];

std::vector<char> area = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
char currentSymbol = 'X';
void connect();
void move_handler(char currentSymbol,
				  boost::asio::ip::tcp::socket &currentSocket);
char checkWin(char symbol);
void restart();

int main() {
	connect();

	boost::asio::write(client1Socket, boost::asio::buffer(area));
	boost::asio::write(client2Socket, boost::asio::buffer(area));

	while (true) {
		if (currentSymbol == 'X') {
			data[0] = 'X';
			boost::asio::write(client1Socket, boost::asio::buffer(data));
			data[0] = '0';
			boost::asio::write(client2Socket, boost::asio::buffer(data));
			move_handler('X', client1Socket);
			currentSymbol = 'O';
		} else {
			data[0] = 'X';
			boost::asio::write(client2Socket, boost::asio::buffer(data));
			data[0] = '0';
			boost::asio::write(client1Socket, boost::asio::buffer(data));
			move_handler('O', client2Socket);
			currentSymbol = 'X';
		}
	}

	return 0;
}

void connect() {
	std::cout << "Server started\n";
	acceptor.accept(client1Socket);
	std::cout << "Client 1 connected\n";
	acceptor.accept(client2Socket);
	std::cout << "Client 2 connected\n";
}

void move_handler(char currentSymbol,
				  boost::asio::ip::tcp::socket &currentSocket) {
	boost::asio::read(currentSocket, boost::asio::buffer(data));

	for (int i = 0; i < area.size(); i++) {
		if (data[0] == area[i]) {
			area[i] = currentSymbol;
			boost::asio::write(client1Socket, boost::asio::buffer(area));
			boost::asio::write(client2Socket, boost::asio::buffer(area));
			char result = checkWin(currentSymbol);

			if (result == 'W') {
				std::cout << "Player " << currentSymbol << " won\n";
				boost::asio::write(currentSocket, boost::asio::buffer("W"));
				if (currentSymbol == 'X') {
					boost::asio::write(client2Socket, boost::asio::buffer("L"));
				} else {
					boost::asio::write(client1Socket, boost::asio::buffer("L"));
				}
				restart();
			} else if (result == 'D') {
				std::cout << "Draw\n";
				boost::asio::write(client1Socket, boost::asio::buffer("D"));
				boost::asio::write(client2Socket, boost::asio::buffer("D"));
				restart();
			} else {
				std::cout << "Continue\n";
			}
		}
	}
}

char checkWin(char symbol) {
	if (area[0] == symbol && area[1] == symbol && area[2] == symbol ||
		area[3] == symbol && area[4] == symbol && area[5] == symbol ||
		area[6] == symbol && area[7] == symbol && area[8] == symbol ||
		area[0] == symbol && area[3] == symbol && area[6] == symbol ||
		area[1] == symbol && area[4] == symbol && area[7] == symbol ||
		area[2] == symbol && area[5] == symbol && area[8] == symbol ||
		area[0] == symbol && area[4] == symbol && area[8] == symbol ||
		area[2] == symbol && area[4] == symbol && area[6] == symbol) {
		return 'W';
	} else if (area[0] != '1' && area[1] != '2' && area[2] != '3' &&
			   area[3] != '4' && area[4] != '5' && area[5] != '6' &&
			   area[6] != '7' && area[7] != '8' && area[8] != '9') {
		return 'D';
	}

	else {
		return 'N';
	}
}

void restart() {
	boost::asio::read(client1Socket, boost::asio::buffer(data));
	boost::asio::read(client2Socket, boost::asio::buffer(data));

	if (data[0] == 'R') {
		area = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
		std::cout << "Restart\n";
	} else {
		client1Socket.close();
		client2Socket.close();
		std::cout << "Server closed\n";
	}

	boost::asio::write(client1Socket, boost::asio::buffer(area));
	boost::asio::write(client2Socket, boost::asio::buffer(area));
}
