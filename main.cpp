// Client part for Server-Client chat. Developed by Mr_Dezz, modified by Emil
#include <iostream> //потоковый ввод-вывод
#include <WinSock2.h> // для работы с сокетами
#include <WS2tcpip.h> // для работы с tcp/ip
#include <stdio.h> // для fgets (это из изначального прототипа, впадлу было переделывать)
#include <vector> // для работы с векторами
#include <windows.h> //для sleep
#include <thread> // Для многопоточности
#include "data_generation.h" // для второго (случайного) типа сообщений	
#include "main.h" // заголовочный файл main


#pragma comment(lib, "ws2_32.lib")

using namespace std;

void NewMessageCheck() { // Создал новую функцию, потому что так создаётся новый поток, сорри!
	while (true) {
		Sleep(100); // чтобы процессор не страдал, будем проверять каждые 0.1 секунды
			packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);
			cout << servBuff.data() << endl;
	}
}

int main(void)
{
	//Key constants
	//const char SERVER_IP[] = "127.0.0.1";					// Enter IPv4 address of Server
	//const short SERVER_PORT_NUM = 1;				// Enter Listening port on Server side
	//short BUFF_SIZE = 1024;					// Maximum size of buffer for exchange info between server and client
	// Key variables for all program
	//fd_set rfds;
	//struct timeval tv;
	//int retval;

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);
	// WinSock initialization
	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Socket initialization

	ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
	}
	else
		cout << "Client socket initialization is OK" << endl;

	// Establishing a connection to Server


	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Connection established SUCCESSFULLY." << endl;



	
	cout << "Enter server password. You have 3 attempts" << endl; // Введите пароль сервера. У вас 3 попытки
	while ((memcmp(servBuff.data(),"correct password",strlen("correct password") )) != 0) { // проверка, правильный ли пароль
		
		if (attempts > 2) { // Если больше 3 неверных попыток - разрываем соединение
			cout << "You have exceeded the number of given attempts.\nWe have reported your IP to your local police station. Prepare your anus." << endl;
			shutdown(ClientSock, SD_BOTH);
			closesocket(ClientSock);
			WSACleanup();
			return 0;
		}
		
		fgets(clientBuff.data(), clientBuff.size(), stdin);
		msglen = strlen(clientBuff.data()) - 1;
		packet_size = send(ClientSock, clientBuff.data(), msglen, 0); // отправляем пароль
		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0); // получаем ответ
		cout << servBuff.data() << endl;
		attempts++;
		
	}
	cout << "Enter username:" << endl;
	fgets(clientBuff.data(), clientBuff.size(), stdin); // спрашиваем юзернейм
	msglen = strlen(clientBuff.data())-1; // получаем размер сообщения
	packet_size = send(ClientSock, clientBuff.data(), msglen, 0);
	cout << "Welcome, " << clientBuff.data()<<endl;
	thread MSG_Check(NewMessageCheck);
	

	while (true) {
		
		
		cout << "Select the type of message you would like to send.\n\t1 - custom message, 2 - generated message" << endl;
		TypeOfMSG = 0;
		while ((TypeOfMSG != 1) && (TypeOfMSG != 2)) {
			cin >> TypeOfMSG;
			if (TypeOfMSG == 1) {
				cout << "You chose type " << TypeOfMSG << endl;
				cout << "Enter your message:" << endl;
				while ((getchar()) != '\n'); //This will consume the '\n' char
				//now you're free to use fgets
				fgets(clientBuff.data(), clientBuff.size(), stdin); // Ввод сообщения
				msglen = strlen(clientBuff.data()) - 1;
				// Check whether client like to stop chatting
				if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') { // здесь проверяем хочет ли юзер перестать общаться
					
					MSG_Check.join();
					shutdown(ClientSock, SD_BOTH);
					closesocket(ClientSock);
					WSACleanup();
					return 0;
				}
				else {
					packet_size = send(ClientSock, clientBuff.data(), msglen, 0); //Отправка сообщения
				}
			}
			else if (TypeOfMSG == 2) {
				cout << "You chose type " << TypeOfMSG << endl;
				cout << "Choose length of your message" << endl;
				cin >> MSGSize; // выбираем длину
				cout << "Choose how many messages to send" << endl;
				cin >> MSG_num; // выбираем количество
				cout << "Choose the periodicity of your message (how often do you send it), in seconds" << endl;
				cin >> P; // выбираем периодичность (как часто в секундах отправлять)
				for (int i = 0; i < MSG_num; i++) {
					Sleep(P * 1000); // задержка в секундах
					cout << "Message #" << i + 1 << " ";
					generate(MSGSize, clientBuff); // генерируем сообщение
					for (int j = 0; j < MSGSize; j++) {
						cout << clientBuff[j]; // выводим его
					}
					cout << "\n";
					packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0); // отправляем
				}
			}
		}
		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			MSG_Check.join();
			return 1;
		}
		
		


		if (packet_size == SOCKET_ERROR) {
			cout << "Can't receive message. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			MSG_Check.join();
			return 1;
		}

	}

	closesocket(ClientSock);
	WSACleanup();
	MSG_Check.join();
	return 0;
}