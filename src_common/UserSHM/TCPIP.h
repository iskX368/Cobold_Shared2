#ifndef TCPIP_SERVER_CLASS_initialized
	#define TCPIP_SERVER_CLASS_initialized

	/////////////////////////////////////////////////////////
	class TCPIP_server_class {
	/////////////////////////////////////////////////////////
	#define TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS (100)
	public:

		volatile SOCKADDR_IN my_addr;
		struct timeval myTime;

		SOCKET		remote_socket[TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS];
		bool		remote_socket_is_alive[TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS];
		SOCKADDR_IN remote_addr[TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS];

		volatile __int32 number_of_remote_sockets;
		volatile SOCKET server_socket;
		volatile __int32 remote_size;
		volatile __int32 LastError;

		char *		buffer;
		__int32		buffersize;

		TCPIP_server_class (__int32 _buffersize);
		~TCPIP_server_class ();

		bool initialize_server(__int32 port_number);
		__int32 get_free_socket_id();
		bool accept_socket(__int32 remote_socket_index);
		__int32 recv_socket(__int32 remote_socket_index,  char* _buffer, __int32 _buffersize);
		__int32 send_socket(__int32 remote_socket_index,  char* _buffer, __int32 number_of_bytes);
		void close_remote_socket(__int32 remote_socket_index);
	};












	/////////////////////////////////////////////////////////
	class TCPIP_thread_class
	/////////////////////////////////////////////////////////
	{
	public:
		volatile CWinThread *wtpTCPIPCThread;
		volatile bool bPleaseTerminateThread;
		volatile bool bThreadIsRunning;
		volatile char command[1000];
		volatile __int32 port;
		volatile bool do_echo;
		CDoubleArray *Parameters;
		TCPIP_server_class * server;

		// constructor
		TCPIP_thread_class(__int32 port);

		// destructor
		~TCPIP_thread_class();
		
		void StartThread();
		void TerminateThread();

		//static UINT TCPIPThread(LPVOID vpClassPointer);
	};

#endif
