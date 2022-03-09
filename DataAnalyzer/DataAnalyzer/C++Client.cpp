#include <sio_client.h>
#include <iostream>
using namespace sio;

void OnMessage(event& ev) {
	std::cout << ev.get_message()->get_string() << std::endl;
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

