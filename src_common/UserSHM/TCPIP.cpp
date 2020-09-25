#include "stdafx.h" 
#include "CoboldUtilities.h"
#include "TCPIP.h"
#include "UserSHM.h"

////////////////////////////////////////////////
// members of TCPIP_server_class:
////////////////////////////////////////////////

#define TCPIP_TIMEOUT_COBOLD 1000

//////////////////////////////////////////////////////////////////////////
// TCPIP_server_class
//////////////////////////////////////////////////////////////////////////

TCPIP_server_class::TCPIP_server_class(__int32 _buffersize) : buffer(nullptr)
{
	this->buffersize = _buffersize;
	remote_size = 0;
	myTime.tv_sec = 0;
	myTime.tv_usec = TCPIP_TIMEOUT_COBOLD;

	for (__int32 i=0; i<TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS; ++i) 
	{
		remote_socket[i] = 0;
		if (remote_socket_is_alive[i])
			close_remote_socket(i);
	}
	server_socket = 0;
	number_of_remote_sockets = 0;

	ASSERT(buffersize > 0);
	buffer = new char[buffersize];
	if(buffer)
		memset(buffer,0,buffersize);
}

TCPIP_server_class::~TCPIP_server_class()
{
	if (buffer)
	{
		delete[] buffer;
		buffer = nullptr;
	}
	for (__int32 i=0;i<number_of_remote_sockets;++i)
	{
		shutdown(remote_socket[i],SD_BOTH);
		closesocket(remote_socket[i]);
		remote_socket[i] = 0;
	}
	WSACleanup();
}

bool TCPIP_server_class::initialize_server(__int32 port_number)
{
	WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);

	u_long ttimeout = htonl(TCPIP_TIMEOUT_COBOLD); //  timeout in ms

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port_number);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	u_long server_is_non_blocking = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ttimeout, sizeof(ttimeout));
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&ttimeout, sizeof(ttimeout));
	if (ioctlsocket(server_socket,FIONBIO,&server_is_non_blocking)) 
		return false;

	if (bind(server_socket, (SOCKADDR *)&my_addr, sizeof(my_addr)) == -1) 
	{
		// error
		closesocket(server_socket);
		return false;
	}

	if (listen(server_socket, 5) == -1)
	{
		// error
		return false;
	}
	return true;
}

__int32 TCPIP_server_class::get_free_socket_id()
{
	__int32 return_id = -1;
	for (__int32 i=0;i<TCPIP_server_MAX_NUMBER_OF_REMOTE_SOCKETS;++i)
	{
		if (!remote_socket_is_alive[i])
		{
			return_id = i;
			break;
		}
	}
	return return_id;
	
}

bool TCPIP_server_class::accept_socket(__int32 remote_socket_index)
{
	remote_size = sizeof(remote_addr[remote_socket_index]);
	SOCKET remote_s =  accept(server_socket, (SOCKADDR *)&remote_addr[remote_socket_index], (__int32*)&remote_size);
	if (remote_s == INVALID_SOCKET) 
	{
		// error
		LastError = WSAGetLastError();
	}

	u_long server_is_non_blocking = 1;
	__int32 timeout = TCPIP_TIMEOUT_COBOLD; // timeout in ms
	if (ioctlsocket(remote_s,FIONBIO,&server_is_non_blocking)) 
		return false;
	setsockopt(remote_s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof timeout);
	linger linger_struct; linger_struct.l_linger = 0;	
	linger_struct.l_onoff = 1;
	setsockopt(remote_s, SOL_SOCKET, SO_LINGER, (char *)&linger_struct, sizeof linger_struct);

	remote_socket[remote_socket_index] = remote_s;
	remote_socket_is_alive[remote_socket_index] = true;
	return true;
}

__int32 TCPIP_server_class::recv_socket(__int32 remote_socket_index,  char* _buffer, __int32 _buffersize)
{
	if (!remote_socket_is_alive[remote_socket_index]) 
		return 0;
	__int32 r = recv(remote_socket[remote_socket_index], _buffer, _buffersize, 0); 
	//if (r <= 0) close_remote_socket(remote_socket_index); 
	return r;
}

__int32 TCPIP_server_class::send_socket(__int32 remote_socket_index,  char* _buffer, __int32 number_of_bytes) 
{
	if (!remote_socket_is_alive[remote_socket_index]) 
		return 0;
	__int32 sent_size = send(remote_socket[remote_socket_index], _buffer,number_of_bytes,0);
	if (sent_size <= 0)
		close_remote_socket(remote_socket_index);
	return sent_size;
}

void TCPIP_server_class::close_remote_socket(__int32 remote_socket_index)
{
	shutdown(remote_socket[remote_socket_index],SD_BOTH);
	closesocket(remote_socket[remote_socket_index]);
	remote_socket_is_alive[remote_socket_index] = false;
}

//////////////////////////////////////////////////////////////////////////
// TCPIPThread
//////////////////////////////////////////////////////////////////////////

static UINT TCPIPThread(LPVOID vpClassPointer)
{
	TCPIP_thread_class *TCPIP_Instance = (TCPIP_thread_class*)vpClassPointer;
	TCPIP_Instance->bThreadIsRunning = true;

	TCPIP_Instance->server = new TCPIP_server_class(1000); // buffersize: 1000 bytes

	ASSERT(TCPIP_Instance->port);

	TCPIP_Instance->server->initialize_server(TCPIP_Instance->port);
		
	CString command = "";
	bool command_is_complete = false;

	while (!TCPIP_Instance->bPleaseTerminateThread)
	{
		__int32 remote_socket_id = TCPIP_Instance->server->get_free_socket_id();

		while (!TCPIP_Instance->bPleaseTerminateThread) 
		{
			//select(maxHandle, &lesesockets, NULL, NULL, &myTime);

			if (!TCPIP_Instance->server->remote_socket_is_alive[remote_socket_id])
			{
				bool ret_value = TCPIP_Instance->server->accept_socket(remote_socket_id);
				if (!ret_value)
				{
					Sleep(500);
					continue;
				}
			}

			if (TCPIP_Instance->bPleaseTerminateThread) 
				break;

			// now receiving data...
			__int32 recv_size = TCPIP_Instance->server->recv_socket(remote_socket_id, TCPIP_Instance->server->buffer, TCPIP_Instance->server->buffersize);

			if (TCPIP_Instance->bPleaseTerminateThread)
				break;

			if (recv_size == 0)
			{
				// connection was gracefully closed by client
				TCPIP_Instance->server->close_remote_socket(remote_socket_id);
				Sleep(500);
				continue;
			}

			if (recv_size < 0)
			{
				// error during receive
				//TCPIP_Instance->bPleaseTerminateThread = true;
				//break;
				Sleep(500);
				continue;
			}

			if (TCPIP_Instance->do_echo)
			{
				__int32 sent_size = TCPIP_Instance->server->send_socket(remote_socket_id, TCPIP_Instance->server->buffer,recv_size);
				if (sent_size == -1) 
				{
					// error while sending
					break;
				}
				else
				{
					//  Bytes send to remote host
				}
			}

			bool end_sequence_found = false;
			while (!end_sequence_found && recv_size>0)
			{
				if (TCPIP_Instance->server->buffer[recv_size-1] == 13 || TCPIP_Instance->server->buffer[recv_size-1] == 10)
				{
					--recv_size;
					end_sequence_found = true;
					command_is_complete = true;
					continue;
				}
				break;
			}
			TCPIP_Instance->server->buffer[recv_size] = 0;

			command += CString(TCPIP_Instance->server->buffer);

			if (command_is_complete)
			{
				command_is_complete = false;
				while (command.GetLength() > 0) 
				{
					if (command.GetAt(command.GetLength()-1) == 13 || command.GetAt(command.GetLength()-1) == 10) 
					{
						command = command.Left(command.GetLength()-1);
						continue;
					}
					break;
				}
				if (command.Left(3).MakeLower() == CString("new") ||
					command.Left(5).MakeLower() == CString("start") ||
					command.Left(4).MakeLower() == CString("stop") ||
					command.Left(3).MakeLower() == CString("sho") ||
					command.Left(4).MakeLower() == CString("exec") ||
					command.Left(4).MakeLower() == CString("paus") ||
					command.Left(4).MakeLower() == CString("call"))
				{
						CString aaa = CString("cmd.exe /C CoboldPCCMD.exe \"") + command + CString("\"");
						system(LPCTSTR(aaa));
				} 
				else if(command.Left(5).MakeLower() == CString("get_p"))
				{
#define number_size	51
					char number[number_size]; number[0] = 0;
					__int32 index = 0;
					for (__int32 i=0;i<command.GetLength();i++) 
					{
						if (command.GetAt(i) >= '0' && command.GetAt(i) <= '9' && index<number_size-1) 
						{
							number[index++] = command.GetAt(i);
							number[index] = 0;
						}
					}
					if (index > 0) 
					{
						double par_value = 0.;
						__int32 par_index = atoi(number);
						if (TCPIP_Instance->Parameters) 
						{
							if (TCPIP_Instance->Parameters->GetSize() > par_index)
								par_value = TCPIP_Instance->Parameters->GetAt(par_index);
						}
						sprintf_s(number,number_size,"%lg",par_value);
#undef number_size
						__int32 sent_size = TCPIP_Instance->server->send_socket(remote_socket_id, number, __int32(strlen(number))+1);
					}
				}
				else if(command.Left(11).MakeLower() == CString("close_tcpip"))
				{
					TCPIP_Instance->bPleaseTerminateThread = true;
					break;
				}
				else 
				{
					command.TrimLeft();
					if (command.GetLength() > 0) 
						ExecuteCommand(command);
				}
				command = "";
			}				
		}
	}

	delete TCPIP_Instance->server;

	TCPIP_Instance->bThreadIsRunning = false;
	AfxEndThread(1,TRUE);
	return TRUE;
}

////////////////////////////////////////////////
// members of TCPIP_thread_class:
////////////////////////////////////////////////

// constructor
TCPIP_thread_class::TCPIP_thread_class(__int32 port_):bPleaseTerminateThread(false),
						do_echo(false),
						bThreadIsRunning(false)  
{
	Parameters = nullptr;
	port = port_;
	server = 0;
}

// destructor
TCPIP_thread_class::~TCPIP_thread_class() 
{
	if(this->bThreadIsRunning) 
	{
		this->bPleaseTerminateThread = true;
		Sleep(50);
	}
	if(this->bThreadIsRunning)
		TerminateThread();
	if (server) 
	{
		delete server;
		server = 0;
	}
}

void TCPIP_thread_class::StartThread() 
{
	this->bPleaseTerminateThread = false;
	this->wtpTCPIPCThread = AfxBeginThread(&TCPIPThread ,(LPVOID)this,THREAD_PRIORITY_NORMAL,0,0,NULL);
}

void TCPIP_thread_class::TerminateThread() 
{
	this->bPleaseTerminateThread = true;
	if (this->bThreadIsRunning) 
	{
		Sleep(10);
		//while (this->bThreadIsRunning) {this->bPleaseTerminateThread = true; Sleep(10);} // XXX not yet working because thread is alread dead. But why?
	}
}

	


