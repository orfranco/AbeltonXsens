#include <sio_client.h>
#include <iostream>
using namespace sio;

void OnMessage(event& ev) {
	std::string msg = ev.get_message()->get_string();
	int buffer = 8;
	double euler_x = std::stod(msg.substr(msg.find("euler_x") + 8, buffer));
	std::cout << "euler x: " << euler_x << std::endl;
}

int main() {
	client* h = new client();
	//todo: change to localhost??
	h->connect("http://127.0.0.1:3001");
	h->socket()->on("recieve-message", &OnMessage);
	while(true){
		//todo:
	}
}

