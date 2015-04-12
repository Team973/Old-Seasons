#include <pthread.h>
#include <unistd.h>
#include "socketClient.hpp"

#include <time.h>
#include <stdlib.h>

namespace frc973 {

class TestListener : public SocketClient::SocketListener {
	private:
		std::string m_listenerName;
	public:
		TestListener(std::string listenerName) {
			m_listenerName = listenerName;
		}
		void OnValueChange(std::string name, std::string newValue) {
			printf("listener '%s' heard '%s' now set to %f\n", m_listenerName.c_str(), name.c_str(), atof(newValue.c_str()));
		}
};

}

void* runNet(void* in)
{
    frc973::SocketClient::Start();
    frc973::SocketClient::Run(NULL);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t netThread;
    pthread_create(&netThread, NULL, runNet, NULL);

	frc973::SocketClient::AddListener("kangaccelff", 
			new frc973::TestListener("listener1"));

	frc973::TestListener* dual
		= new frc973::TestListener("listener2");
	frc973::SocketClient::AddListener("a", dual);
	frc973::SocketClient::AddListener("b", dual);

    while (1) {
        sleep(4);
    }

    return 0;
}
