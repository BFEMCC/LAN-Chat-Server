#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_BUF_SIZE 1024 
#define VERSION L"版本：1.0"
#define EDITFONTSIZE 20 //输入框字号
#define BTNFONTSIZE 20 //按钮字号
#define FONT L"微软雅黑"
#define MAX_MSG_SIZE 1024  // 定义最大消息长度
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <regex>
#include <string>
#include <fstream>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <io.h>
#include <vector>
#include"HiEasyX/HiEasyX.h"
#include"依赖函数.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
using std::cout;
using std::wcout;
using std::endl;
//int main()
//{
//    ////1、加载网络库
//    //if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    //{
//    //    memset(ErrorMessage, 0, sizeof(ErrorMessage));
//    //    swprintf_s(ErrorMessage, L"Windows网络库初始化失败！错误码：%d", WSAGetLastError());
//    //    MessageBox(nullptr, ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
//    //    exit(-1);
//    //}
//    hiex::DelayFPS(60);
//    hiex::Window wnd;
//    wnd.Create(640, 480, 1 | 4, L"");
//
//    hiex::SysComboBox combobox;//下拉菜单框
//    combobox.PreSetStyle({ false, false, true });    ///< 是否总是显示列表
//                                                     ///< 是否可编辑
//                                                     ///< 是否自动排序
//    combobox.Create(wnd.GetHandle(), 150, 50, 200, 200, L"");
//    std::wstring planet[9] = { L"1" ,L"2" ,L"3" ,L"4" ,L"5" ,L"6" ,L"7" ,L"8" ,L"9" };
//    for (auto& i : planet) combobox.AddString(i);//添加项
//    
//    //combobox.SelectString(L"2");
//    combobox.SetSel(5);//设置指定项
//
//    std::cout << combobox.GetCount() << std::endl;//获取项数
//
//    combobox.DeleteString(7);//删除项
//    
//    //combobox.RegisterSelMessage();//设置消息响应函数
//
//    std::cout << combobox.GetSel() << std::endl;
//
//
//
//    hiex::init_end();
//    return 0;
//}

WSADATA wsaData;
SOCKET ServerSocket;
struct sockaddr_in  ServerAddr;
wchar_t ErrorMessage[128];
std::string LocalWifiIP;
fd_set Readfdset;  // 读 套接字集合
char recvbuf[MAX_MSG_SIZE];
char SendBuf[MAX_MSG_SIZE];
int EptyLength1 = 0;
int EptyLength2 = 0;
int ChoseSocket;
int SendNum;
std::wstring SendEditContent = L"";
std::wstring SendWstr = L"";
std::string SendStr = "";
const char* SendChar = nullptr;
int SendLength;
std::string LocalIPStr = "";
std::ofstream filew;
std::ifstream filer;
std::ofstream filed;
std::string strrecv = "";
std::wstring wsrtrecv = L"";
HANDLE hThread = NULL;
std::vector<SOCKET> ClientSocketfd;							// 使用vector容器记录和管理客户端socket
std::vector<std::wstring> ParameterMessage;					// 使用vector容器存储提示信息

struct ThreadParameter //线程函数参数
{
	SOCKET socketfd;
	hiex::SysComboBox combobox;
	hiex::Window wnd;
	hiex::SysEdit recvedit;
	hiex::SysEdit promptedit;
	hiex::SysStatic totalclients;
};

void closeOperations(bool closeFile, bool closeThread, bool cleanInt, bool exitProgram)//关闭文件、线程、网络库、退出相关
{
	if (closeFile)
	{
		if (filew.is_open())
		{
			filew.close();
		}
		if (filer.is_open())
		{
			filer.close();
		}
	}

	if (closeThread && hThread != NULL)
	{
		CloseHandle(hThread);
	}

	if (cleanInt)
	{
		closesocket(ServerSocket);
		WSACleanup();
	}

	if (exitProgram)
	{
		exit(0);
	}
}


void WINAPI ThreadIoSelect(LPVOID lpParam)
{
	ThreadParameter* Parameter = (ThreadParameter*)lpParam;		// 参数强转
	std::wstring _ParameterMessage = L"";
	fd_set readfdset{};											// 存储可读性socket集合
	SOCKET listen_socket;										// 监听socket
	int selreturn;												// select()返回值
	SOCKET data_socket;											// 数据socket
	int recvnum;												// 接收的字符数量
	listen_socket = (SOCKET)Parameter->socketfd;				// 将参数强制赋值给监听socket的文件描述符

	while (1)  // 循环监视
	{
		FD_ZERO(&readfdset);									// 清空可读性文件描述符集合
		readfdset = Readfdset;									// 将全局的，需要监视的文件描述符集合，赋值给当前的文件描述符集合
		Parameter->totalclients.SetText(L"客户端总数:" + stringTowstring(std::to_string(ClientSocketfd.size())));
		selreturn = select(0, &readfdset, NULL, NULL, NULL);	// I/O多路复用，阻塞等待有文件描述符准备好进行I/O操作
		if (0 == selreturn)										// 超时，继续等待，继续监视
		{
			continue;
		}
		else if (0 > selreturn)									// 失败，退出循环
		{
			printf("select监视失败: ");

			break;
		}

		for (int i = 0; i < readfdset.fd_count; i++)					// 遍历准备好进行I/O操作的文件描述符集合
		{
			if (readfdset.fd_array[i] == listen_socket)					// 如果是监听socket的文件描述符
			{
				data_socket = accept(listen_socket, NULL, NULL);		// 接受连接请求，获取数据socket的文件描述符
				if (data_socket <= 0)									// 失败，将监听socket的文件描述符从文件描述符集合中清除并继续循环
				{
					//printf("socketfd=%d,接受连接失败: ", listen_socket);
					FD_CLR(listen_socket, &Readfdset);
					continue;
				}
				else													// 成功，将数据socket的文件描述符加入文件描述符集合，输出连接成功的提示信息
				{
					ClientSocketfd.push_back(data_socket);				//加入容器
					Parameter->combobox.AddString(stringTowstring(std::to_string(data_socket)));
																		//将新加入的客户端添加进下拉列表
					Parameter->combobox.SelectString(stringTowstring(std::to_string(data_socket)));
																		//重置下拉列表设置为第一个选项

					ParameterMessage.push_back(stringTowstring(GetCurrentTimeStr()) + L"_客户端:" + stringTowstring(std::to_string(data_socket)) + L"加入\r\n");
					for (const auto& i: ParameterMessage)
					{
						_ParameterMessage += i;
					}
					Parameter->promptedit.SetText(_ParameterMessage);
					_ParameterMessage.clear();
					FD_SET(data_socket, &Readfdset);
				}
			}
			else  // 如果是数据socket的文件描述符
			{
				memset(recvbuf, 0, MAX_MSG_SIZE);  // 清空接收缓冲区
				recvnum = recv(readfdset.fd_array[i], recvbuf, MAX_MSG_SIZE, 0);  // 接收数据
				if (recvnum > 0)// 成功接收到消息
				{
					while (true)//等待文件关闭
					{
						if (!filew.is_open() && !filer.is_open())
						{
							filew.open("ChatHistory.txt", std::ios::app); // 打开文件，以追加的形式写入
							if (!filew.is_open()) // 如果文件打开失败
							{
								MessageBox(Parameter->wnd.GetHandle(), L"系统错误，文件操作失败\n程序可继续运行，但不会显示和保存历史消息", L"错误", MB_OK | MB_ICONERROR);
								Parameter->recvedit.SetText(charTowstring(recvbuf));
								break;
							}
							else
							{
								strrecv = "";
								strrecv = GetCurrentTimeStr() + "_" + getLocalSocketAddrName(ServerSocket) + "_[接收_来自]_" + GetClientInfo(readfdset.fd_array[i]) + " >>：" + charToString(recvbuf);
								filew << strrecv << std::endl;//接收到的内容写入文件保存并换行
								filew.close();
								Sleep(100);
								while (true)
								{
									if (!filew.is_open() && !filer.is_open())
									{
										filer.open("ChatHistory.txt", std::ios::in); // 打开文件，以读取的形式读取
										if (!filer.is_open()) // 如果文件打开失败
										{
											MessageBox(Parameter->wnd.GetHandle(), L"系统错误，文件操作失败\n历史消息已保存，程序可继续运行，但不会显示历史消息", L"错误", MB_OK | MB_ICONERROR);
											Parameter->recvedit.SetText(charTowstring(recvbuf));
											break;
										}
										else
										{
											wsrtrecv = L"";
											while (std::getline(filer, strrecv)) // 逐行读取文件内容
											{
												wsrtrecv += stringTowstring(strrecv);
												wsrtrecv += L"\r\n";
											}
											filer.close();
											Parameter->recvedit.SetText(wsrtrecv);
											break;
										}
									}
									Sleep(5);
								}
								break;
							}
						}
						Sleep(5);
					}
					continue;
				}
				else // 失败，客户端断开连接
				{
					auto it = std::find(ClientSocketfd.begin(), ClientSocketfd.end(), readfdset.fd_array[i]);//遍历找到断开的socket
					ParameterMessage.push_back(stringTowstring(GetCurrentTimeStr()) + L"_客户端:" + stringTowstring(std::to_string(*it)) + L"断开\r\n");
					for (const auto& i : ParameterMessage)
					{
						_ParameterMessage += i;
					}
					Parameter->promptedit.SetText(_ParameterMessage);
					_ParameterMessage.clear();
					auto index = std::distance(ClientSocketfd.begin(), it);//获取下标索引
					Parameter->combobox.DeleteString(index);		//从下拉列表里删除
					Parameter->combobox.SetSel(0);					//设置当前选项为第一个
					ClientSocketfd.erase(it);						//从vector里删除

					FD_CLR(readfdset.fd_array[i], &Readfdset);		//将数据socket的文件描述符从文件描述符集合中清除
					closesocket(readfdset.fd_array[i]);				//关闭文件描述符
					continue;
				}
			}
		}
	}

}


int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		memset(ErrorMessage, 0, sizeof(ErrorMessage));
		swprintf_s(ErrorMessage, L"Windows网络库初始化失败！错误码：%d", WSAGetLastError());
		MessageBox(nullptr, ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
		exit(-1);
	}
	ServerSocket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == ServerSocket)
	{
		WSACleanup();
		memset(ErrorMessage, 0, sizeof(ErrorMessage));
		swprintf_s(ErrorMessage, L"套接字创建失败！错误码：%d", GetLastError());
		MessageBox(nullptr, ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
		exit(-1);
	}
	//3、绑定服务器socket到指定端口//绑定到本地无线局域网IP，端口不指定
	LocalWifiIP = getWirelessAdapterIPV4Address();
	memset(&ServerAddr, 0, sizeof(struct sockaddr_in));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(LocalWifiIP.c_str());
	//ServerAddr.sin_port = htons(32119);
	int r = bind(ServerSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (-1 == r)
	{
		closesocket(ServerSocket);
		WSACleanup();
		memset(ErrorMessage, 0, sizeof(ErrorMessage));
		swprintf_s(ErrorMessage, L"服务器地址和端口绑定错误！错误码：%d", GetLastError());
		MessageBox(nullptr, ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
		exit(-1);
	}
	std::cout << getLocalSocketAddrName(ServerSocket) << std::endl;//输出端口和地址

/*-----------------------------------------------------------------------------------------*/
	//主界面UI
	hiex::Window mainwnd(1000, 640, EX_NOMINIMIZE|1, L"收&发消息");

	hiex::SysEdit RecvEdit;
	RecvEdit.PreSetStyle({ true, false, true, true, false, false, });
	RecvEdit.Create(mainwnd.GetHandle(), 10, 10, 700, 300);
	RecvEdit.SetFont(20, 0, FONT);
	RecvEdit.ReadOnly(true);

	hiex::SysEdit SendEdit;
	SendEdit.PreSetStyle({ true, false, true, true, false, false, });
	SendEdit.Create(mainwnd.GetHandle(), 10, 320, 700, 150);
	SendEdit.SetFont(EDITFONTSIZE, 0, FONT);
	SendEdit.SetText(L"");
	SendEdit.SetMaxTextLength(MAX_BUF_SIZE - 4);
	EptyLength1 = SendEdit.GetTextLength();
	EptyLength2 = SendEdit.GetText().length();

	hiex::SysEdit PromptEdit;
	PromptEdit.PreSetStyle({ true, false, true, true, false, false, });
	PromptEdit.Create(mainwnd.GetHandle(), 720, 10, 270, 460);
	PromptEdit.SetFont(18, 0, FONT);
	PromptEdit.ReadOnly(true);

	hiex::SysStatic TotalClients;
	TotalClients.Create(mainwnd.GetHandle(), 800, 480, 200, 20);
	TotalClients.SetFont(18, 0, FONT);
	TotalClients.Center(false);
	TotalClients.SetText(L"");

	hiex::SysStatic ServerInformation;
	ServerInformation.Create(mainwnd.GetHandle(), 800, 500, 200, 40);
	ServerInformation.SetFont(18, 0, FONT);
	ServerInformation.Center(false);
	ServerInformation.SetText(L"当前服务器地址和端口:\r\n" + stringTowstring(getLocalSocketAddrName(ServerSocket)));


	hiex::SysButton SendBtn;
	SendBtn.Create(mainwnd.GetHandle(), 680, 490, 100, 30, L"发 送");
	SendBtn.SetFont(BTNFONTSIZE, 0, FONT);

	hiex::SysButton ExitBtn;
	ExitBtn.Create(mainwnd.GetHandle(), 580, 490, 100, 30, L"退 出");
	ExitBtn.SetFont(BTNFONTSIZE, 0, FONT);

	hiex::SysButton SaveAndExit;
	SaveAndExit.Create(mainwnd.GetHandle(), 480, 490, 100, 30, L"保存并退出");
	SaveAndExit.SetFont(BTNFONTSIZE, 0, FONT);

	hiex::SysButton ClearHistory;
	ClearHistory.Create(mainwnd.GetHandle(), 380, 490, 100, 30, L"清空聊天记录");
	ClearHistory.SetFont(BTNFONTSIZE, 0, FONT);

	hiex::SysButton DeleteChatLogFile;
	DeleteChatLogFile.Create(mainwnd.GetHandle(), 280, 490, 100, 30, L"删除记录文件");
	DeleteChatLogFile.SetFont(BTNFONTSIZE, 0, FONT);

	hiex::SysComboBox ClientUser;
	ClientUser.PreSetStyle({ false, false, false });
	ClientUser.Create(mainwnd.GetHandle(), 680, 540, 100, 30);
	ClientUser.SetFont(BTNFONTSIZE, 0, FONT);
	
	hiex::SysGroupBox BtnArea;
	BtnArea.Create(mainwnd.GetHandle(), 270, 470, 520, 60);
	BtnArea.SetFont(18, 0, FONT);
	BtnArea.SetText(L"功能区");

	hiex::SysEdit Developer;
	Developer.PreSetStyle({ true, false, false, false, false, false, });
	Developer.Create(mainwnd.GetHandle(), 10, 478, 255, 105);
	Developer.SetFont(16, 0, FONT);
	Developer.SetText(
		L"B站主页：https://b23.tv/vLGKBCp\r\n"
		L"邮箱：A1840025658@outlook.com\r\n"
		L"GitHub:\r\n"
		L"暂未上传\r\n"
		L"Gitee:\r\n"
		L"暂未上传");
	Developer.ReadOnly(true);
	hiex::SysStatic Tips;
	Tips.Create(mainwnd.GetHandle(), 10, 587, 230, 60);
	Tips.SetFont(18, 0, FONT);
	Tips.SetText(VERSION L"\r\n"
		L"感谢使用，期待关注！\r\n"
		L"本软件完全免费！若您收费获得请举报商家");

/*-----------------------------------------------------------------------------------------*/
	//4、监听服务器socket//5、初始化需要监视的集合Readfdset，将服务器socket加入其中	//6、创建IO复用select线程
	r = listen(ServerSocket, 10);
	if (-1 == r)
	{
		closesocket(ServerSocket);
		WSACleanup();
		memset(ErrorMessage, 0, sizeof(ErrorMessage));
		swprintf_s(ErrorMessage, L"监听失败！错误码：%d", GetLastError());
		MessageBox(nullptr, ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
		exit(-1);
	}	
	FD_ZERO(&Readfdset);
	FD_SET(ServerSocket, &Readfdset);
	ThreadParameter Parameter = { ServerSocket, ClientUser, mainwnd, RecvEdit, PromptEdit ,TotalClients };
	hThread = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)ThreadIoSelect, (LPVOID)&Parameter, 0, NULL);
	if (hThread == NULL)
	{
		memset(ErrorMessage, 0, sizeof(ErrorMessage));
		swprintf_s(ErrorMessage, L"系统错误\n线程创建失败！错误码：%d\n请尝试重新运行程序", GetLastError());
		MessageBox(mainwnd.GetHandle(), ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
		mainwnd.CloseWindow();
		closesocket(ServerSocket);
		WSACleanup();
		exit(-1);
	}
	Sleep(50);

	//7、发送及其他功能
	LocalIPStr = getLocalSocketAddrName(ServerSocket);
	while (mainwnd.IsAlive())
	{
		Sleep(50);

		if (SendBtn.IsClicked())//点击发送按钮
		{
			if (EptyLength1 == SendEdit.GetTextLength() || EptyLength2 == SendEdit.GetText().length())
			{
				MessageBox(mainwnd.GetHandle(), L"不可发送空内容！", L"警告", MB_OK | MB_ICONERROR);
				continue;
			}
			if (EptyLength1 != SendEdit.GetTextLength() && EptyLength2 != SendEdit.GetText().length())
			{
				SendWstr = SendEdit.GetText();
				SendStr = wstringTostring(SendWstr);
				SendChar = SendStr.c_str();
				wstrToint(ClientUser.GetText(), &ChoseSocket);
				SendLength = send(ChoseSocket, SendChar, strlen(SendChar), 0);
				if (SOCKET_ERROR == SendLength || SendLength == 0)
				{
					memset(ErrorMessage, 0, sizeof(ErrorMessage));
					swprintf_s(ErrorMessage, L"发送失败！错误码：%d", WSAGetLastError());
					MessageBox(mainwnd.GetHandle(), ErrorMessage, L"错误", MB_OK | MB_ICONERROR);
					continue;
				}
				else
				{
					SendEdit.SetText(L"");
					SendStr = GetCurrentTimeStr() + "_" + LocalIPStr + "_[向]_" + GetClientInfo(ChoseSocket) +"_[发送]>>：" + wstringTostring(SendWstr);
					while (true)//等待文件关闭
					{
						if (!filew.is_open() && !filer.is_open())
						{
							filew.open("ChatHistory.txt", std::ios::app);
							if (!filew.is_open())
							{
								MessageBox(mainwnd.GetHandle(), L"系统错误，文件操作失败\n程序可继续运行，但不会显示和保存历史消息", L"错误", MB_OK | MB_ICONERROR);
								RecvEdit.SetText(stringTowstring(SendStr));
								break;
							}
							else
							{//打开成功，写入发送内容，保存聊天记录
								filew << SendStr << std::endl;
								filew.close();
								Sleep(100);
								while (true)//等待文件关闭
								{
									if (!filew.is_open() && !filer.is_open())
									{
										filer.open("ChatHistory.txt", std::ios::in); // 打开文件，以读取的形式读取
										if (!filer.is_open()) // 如果文件打开失败
										{
											MessageBox(mainwnd.GetHandle(), L"系统错误，文件操作失败\n历史消息已保存，程序可继续运行，但不会显示历史消息", L"错误", MB_OK | MB_ICONERROR);
											RecvEdit.SetText(stringTowstring(SendStr));
											break;
										}
										else
										{
											strrecv = "";
											wsrtrecv = L"";
											while (std::getline(filer, strrecv)) // 逐行读取文件内容
											{
												wsrtrecv += stringTowstring(strrecv);
												wsrtrecv += L"\r\n";
											}
											filer.close();
											RecvEdit.SetText(wsrtrecv);
											break;
										}
									}
									Sleep(5);
								}
								break;
							}
						}
						Sleep(5);
					}
					continue;
				}
			}
		}

		if (ExitBtn.IsClicked())//点击退出按钮
		{
			if (MessageBox(mainwnd.GetHandle(), L"是否确认退出？", L"提示", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
			{
				Sleep(50);
				if (_access("ChatHistory.txt", 0) == 0)
				{
					if (MessageBox(mainwnd.GetHandle(), L"是否保存聊天记录？\r\n提示：已清空的记录不会保存", L"提示", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
					{
						closeOperations(true, true, true, true);
					}
					closeOperations(true, true, true, false);
					filed.open("ChatHistory.txt", std::ofstream::trunc);
					filed.close();
					exit(0);
				}
				closeOperations(true, true, true, true);
			}
			continue;
		}

		if (SaveAndExit.IsClicked())//点击保存并退出按钮
		{
			if (MessageBox(mainwnd.GetHandle(), L"是否确认保存聊天记录并退出？", L"提示", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
			{
				if (_access("ChatHistory.txt", 0) == 0)
				{
					closeOperations(true, true, true, true);
				}
				else
				{
					MessageBox(mainwnd.GetHandle(), L"记录文件不存在或已删除", L"提示", MB_OK | MB_ICONEXCLAMATION);
					exit(0);
				}
			}
			continue;
		}

		if (ClearHistory.IsClicked())//清空聊天记录
		{
			if (_access("ChatHistory.txt", 0) == 0)//如果文件存在
			{
				if (MessageBox(mainwnd.GetHandle(), L"是否确认清空聊天记录？", L"提示", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
				{
					closeOperations(true, false, false, false);
					filed.open("ChatHistory.txt", std::ofstream::trunc);
					filed.close();
					RecvEdit.SetText(L"");
				}
				continue;
			}
			else
			{
				MessageBox(mainwnd.GetHandle(), L"文件不存在或已删除", L"提示", MB_OK | MB_ICONERROR);
				continue;
			}
		}

		if (DeleteChatLogFile.IsClicked())//点击删除记录文件按钮
		{
			while (true)//等待文件关闭
			{
				if (!filew.is_open() && !filer.is_open())
				{
					if (_access("ChatHistory.txt", 0) == 0)//如果记录存在
					{
						if (MessageBox(mainwnd.GetHandle(), L"是否确认删除？", L"提示", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
						{
							if (remove("ChatHistory.txt") == 0)
							{
								RecvEdit.SetText(L"");
								MessageBox(mainwnd.GetHandle(), L"删除成功\r\n聊天记录文件已删除", L"提示", MB_OK | MB_ICONINFORMATION);
								break;
							}
							else
							{
								MessageBox(mainwnd.GetHandle(), L"删除失败\r\n文件不存在或已删除", L"提示", MB_OK | MB_ICONERROR);
								break;
							}
						}
						break;
					}
					else
					{
						MessageBox(mainwnd.GetHandle(), L"文件不存在或已删除", L"提示", MB_OK | MB_ICONERROR);
						break;
					}
				}
				Sleep(5);
			}
			continue;
		}
	}

	//直接点X，关闭窗口
	if (!mainwnd.IsAlive())
	{
		if (_access("ChatHistory.txt", 0) == 0) //如果记录文件存在
		{
			if (MessageBox(nullptr, L"是否保存聊天记录？\r\n提示：已清空的记录不会保存", L"提示", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
			{
				closeOperations(true, true, true, true);
			}
			else
			{
				closeOperations(true, true, true, false);
				filed.open("ChatHistory.txt", std::ofstream::trunc);
				filed.close();
				exit(0);
			}
		}
		else
		{
			closeOperations(true, true, true, true);
		}
	}
	
	return 0;
}




