#ifndef EASYHOOK_HPP
#define EASYHOOK_HPP

#include <Windows.h>
#include <iostream> // remove this include for no debug-info

class EasyHook
{
private:
	BYTE* source;
	BYTE* destination;
	int length;
	void* allocation = nullptr;
	BYTE* gateway = nullptr;
	bool working;
	bool Hook();
public:
	EasyHook(BYTE* source, BYTE* destination, const int length);
	EasyHook(HMODULE hModule, LPCSTR lpProcName, BYTE* destination, const int length);
	bool Unhook();
	BYTE* GetSource();
	BYTE* GetDestination();
	int GetLength();
	void* GetAllocation();
	BYTE* GetGateway();
	bool IsWorking();
};

#endif