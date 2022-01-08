#include "EasyHook.hpp"


EasyHook::EasyHook(BYTE* source, BYTE* destination, const int length)
{
#ifdef _IOSTREAM_
	std::cout << "Creating hook" << std::endl;
#endif

	this->source = source;
	this->destination = destination;
	this->length = length;

	if (this->length < 5)
	{
		this->working = false;
#ifdef _IOSTREAM_
		std::cout << "Failed to hook" << std::endl;
#endif
		return;
	}

	this->Hook();
}

EasyHook::EasyHook(HMODULE hModule, LPCSTR lpProcName, BYTE* destination, const int length)
{
#ifdef _IOSTREAM_
	std::cout << "Creating hook" << std::endl;
#endif
	this->source = reinterpret_cast<BYTE*>(GetProcAddress(hModule, lpProcName));
	this->destination = destination;
	this->length = length;

	if (this->length < 5)
	{
		this->working = false;
#ifdef _IOSTREAM_
		std::cout << "Failed to hook" << std::endl;
#endif
		return;
	}
	
	this->Hook();
}

bool EasyHook::Hook()
{
	// Greate Gateway
	this->allocation = VirtualAlloc(0, this->length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	this->gateway = reinterpret_cast<BYTE*>(this->allocation);

	memcpy_s(this->gateway, this->length, this->source, this->length);

	uintptr_t gatewayRelativeAdd = this->source - this->gateway - 5;

	*(this->gateway + this->length) = 0xE9;
	*reinterpret_cast<uintptr_t*>(this->gateway + this->length + 1) = gatewayRelativeAdd;

	// Detour
	DWORD protection;
	VirtualProtect(this->source, this->length, PAGE_EXECUTE_READWRITE, &protection);

	uintptr_t relativeAddress = this->destination - this->source - 5;

	*this->source = 0xE9;
	*reinterpret_cast<uintptr_t*>(this->source + 1) = relativeAddress;

	VirtualProtect(this->source, this->length, protection, &protection);

	this->working = true;
#ifdef _IOSTREAM_
	std::cout << "Successfully hooked." << " Source: " << reinterpret_cast<uintptr_t*>(this->source) << " /Destination: " << reinterpret_cast<uintptr_t*>(this->destination) << " /Gateway: " << reinterpret_cast<uintptr_t*>(this->gateway) << " /Length: " << this->length << std::endl;
#endif
	return this->working;
}

bool EasyHook::Unhook()
{
	DWORD protection;
	VirtualProtect(this->source, this->length, PAGE_EXECUTE_READWRITE, &protection);

	for (int i = 0; i < this->length; i++)
	{
		*reinterpret_cast<BYTE*>(this->source + i) = *reinterpret_cast<BYTE*>(this->gateway + i);
	}

	VirtualProtect(this->source, this->length, protection, &protection);

	if (this->allocation)
		VirtualFree(this->allocation, 0, MEM_RELEASE);
	else if (this->gateway)
		VirtualFree(this->gateway, 0, MEM_RELEASE);

	return true;
}

BYTE* EasyHook::GetSource()
{
	return this->source;
}

BYTE* EasyHook::GetDestination()
{
	return this->destination;
}

int EasyHook::GetLength()
{
	return this->length;
}

void* EasyHook::GetAllocation()
{
	return this->allocation;
}

BYTE* EasyHook::GetGateway()
{
	return this->gateway;
}

bool EasyHook::IsWorking()
{
	return this->working;
}
