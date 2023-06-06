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
 * @brief       char����תstring
 * @param[in]   char�����׵�ַ
 * @return      ת�����string�ַ���
*/
std::string charToString(char* charArray);

/**
 * @brief       �������ֵ�wstringתint
 * @param[in]   ��Ҫת����wstring
 * @param[in]   �洢ת�������int��ַ
 * @return      �Ƿ�ת���ɹ�
*/
bool wstrToint(std::wstring str, int* intstr);

/**
 * @brief       char����תwstring
 * @param[in]   char�����׵�ַ
 * @return      wstring�ַ���
*/
std::wstring charTowstring(char* charstr);

/**
 * @brief       string�ַ���תwstring�ַ���
 * @param[in]   string�ַ���
 * @return      wstring�ַ���
*/
std::wstring stringTowstring(std::string str);

/**
 * @brief       wstringתstring
 * @param[in]   wstring�ַ���
 * @return      string�ַ���
*/
std::string wstringTostring(std::wstring wstr);

/**
 * @brief       ��ȡ����ʱ��
 * @brief       �ַ�����ʽ����"[��/��/��_ʱ:��:��]"
 * @param[in]
 * @return      string�ַ���
*/
std::string GetCurrentTimeStr();

/**
 * @brief       ��ȡ�Զ˵�IP��ַ�Ͷ˿ڣ��Է���
 * @brief       ���ַ�����ʽ���أ�ʾ����"[127.0.0.1_8080]"
 * @param[in]   SOCKET�׽���
 * @return      string�ַ���
*/
std::string GetClientInfo(SOCKET sock);

/**
 * @brief       ��ȡ�������߾�������wifi��IP��ַ
 * @brief       ���ַ�����ʽ���أ�ʾ����"127.0.0.1"
 * @param[in]
 * @return      string�ַ���
*/
std::string getWirelessAdapterIPV4Address();

/**
 * @brief       ��ȡ�������ʱʹ�õĵĵ�ַ�Ͷ˿ڣ����أ�
 * @brief       �ַ�����ʽ���ص�ַ�Ͷ˿ڣ�ʾ����"[127.0.0.1_8080]"
 * @param[in]   ʹ�û�󶨵�SOCKET
 * @return      string�ַ���
*/
std::string getLocalSocketAddrName(SOCKET socketFd);

/**
 * @brief       �ж�ip��ַ�Ƿ�Ϸ�
 * @param[in]   wstring�ַ�����ʽ��ip��ַ
 * @return      �Ƿ�Ϸ�
*/
bool Is_Valid_IP(const std::wstring& ip);