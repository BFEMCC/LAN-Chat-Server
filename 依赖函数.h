#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MIB_IF_TYPE_IEEE80211 71
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
#include"HiEasyX/HiEasyX.h"

/**
 * @brief       char数组转string
 * @param[in]   char数组首地址
 * @return      转换后的string字符串
*/
std::string charToString(char* charArray);

/**
 * @brief       将仅数字的wstring转int
 * @param[in]   需要转换的wstring
 * @param[in]   存储转换结果的int地址
 * @return      是否转换成功
*/
bool wstrToint(std::wstring str, int* intstr);

/**
 * @brief       char数组转wstring
 * @param[in]   char数组首地址
 * @return      wstring字符串
*/
std::wstring charTowstring(char* charstr);

/**
 * @brief       string字符串转wstring字符串
 * @param[in]   string字符串
 * @return      wstring字符串
*/
std::wstring stringTowstring(std::string str);

/**
 * @brief       wstring转string
 * @param[in]   wstring字符串
 * @return      string字符串
*/
std::string wstringTostring(std::wstring wstr);

/**
 * @brief       获取本机时间
 * @brief       字符串形式返回"[年/月/日_时:分:秒]"
 * @param[in]
 * @return      string字符串
*/
std::string GetCurrentTimeStr();

/**
 * @brief       获取对端的IP地址和端口（对方）
 * @brief       以字符串形式返回，示例："[127.0.0.1_8080]"
 * @param[in]   SOCKET套接字
 * @return      string字符串
*/
std::string GetClientInfo(SOCKET sock);

/**
 * @brief       获取本地无线局域网（wifi）IP地址
 * @brief       以字符串形式返回，示例："127.0.0.1"
 * @param[in]
 * @return      string字符串
*/
std::string getWirelessAdapterIPV4Address();

/**
 * @brief       获取软件运行时使用的的地址和端口（本地）
 * @brief       字符串形式返回地址和端口，示例："[127.0.0.1_8080]"
 * @param[in]   使用或绑定的SOCKET
 * @return      string字符串
*/
std::string getLocalSocketAddrName(SOCKET socketFd);

/**
 * @brief       判断ip地址是否合法
 * @param[in]   wstring字符串形式的ip地址
 * @return      是否合法
*/
bool Is_Valid_IP(const std::wstring& ip);