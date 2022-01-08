#include <Windows.h>
#include <iostream>

#include "EasyHook.hpp"


typedef BOOL(__stdcall* wglSwapBuffers_template)(HDC hdc);
wglSwapBuffers_template wglSwapBuffers_original;
BOOL __stdcall wglSwapBuffers_detour(HDC hdc)
{
    std::cout << "Hello, world!" << std::endl;

	return wglSwapBuffers_original(hdc);
}

DWORD WINAPI Initialize(HINSTANCE hInstance)
{
    FILE* pFile;
    AllocConsole();
    freopen_s(&pFile, "CONOUT$", "w", stdout);

    HMODULE opengl32_dll = GetModuleHandle("opengl32.dll");

    if (!opengl32_dll)
        return 1;
    EasyHook wglSwapBuffers_hook = EasyHook(opengl32_dll, "wglSwapBuffers", reinterpret_cast<BYTE*>(wglSwapBuffers_detour), 5);
    wglSwapBuffers_original = reinterpret_cast<wglSwapBuffers_template>(wglSwapBuffers_hook.GetGateway());

    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        Sleep(5);
    }

    if(wglSwapBuffers_hook.IsWorking())
        wglSwapBuffers_hook.Unhook();
    if (pFile)
        fclose(pFile);
    FreeConsole();
    FreeLibraryAndExitThread(hInstance, 0);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        HANDLE hThread;
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        hThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialize), hinstDLL, NULL, nullptr);
        if (hThread)
            CloseHandle(hThread);
        break;
    default:
        break;
    }
    return TRUE;
}
