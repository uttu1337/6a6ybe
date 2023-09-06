#pragma once
int TypeOfMSG; // Тип сообщения. 1 - кастомное (вводится пользователем),
string message;			   // 2 - генерируемое (просто задаем настройки)
int MSGSize; //длина сообщения
int P; // период (как часто типа)
int MSG_num; //количество сообщений
char nickname; // никнейм пользовтеля
const char SERVER_IP[] = "127.0.0.1";					// Enter IPv4 address of Server
const short SERVER_PORT_NUM = 1;						// Enter Listening port on Server side
short BUFF_SIZE = 1024;			// Maximum size of buffer for exchange info between server and client

vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);// Buffers for sending and receiving data
short packet_size = 0; // The size of sending / receiving packet in bytes
short int msglen; // длина отправляемого сообщения
int attempts = 0; // число попыток
int erStat;										// For checking errors in sockets functions
fd_set rfds;
struct timeval tv;
int retval;
SOCKET ClientSock;
sockaddr_in servInfo;