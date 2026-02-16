#pragma once

#include <Windows.h>
#include <list>

#ifdef _WIN64
#define GetPage(Ptr) ((uintptr_t)Ptr & 0xFFFFFFFFFFFFF000)
#else
#define GetPage(Ptr) ((uintptr_t)Ptr & 0xFFFFF000)
#endif

enum E_CallbackFlags : int
{
	CF_READ = 1 << 0,
	CF_WRITE = 1 << 1,
};

enum E_ExceptionInfoZero : int
{
	EIZ_READ,
	EIZ_WRITE,
	EIZ_DEPVIO = 8,
};

#define PGHAPI __fastcall
typedef void( PGHAPI* PGHCALLBACK )( PCONTEXT pCtx, E_CallbackFlags eType );

class C_CallbackInfo
{
public:
	const void* m_pAddress {};
	PGHCALLBACK m_pCallback {};
	E_CallbackFlags m_eFlags {};
};

class C_HookInfo
{
public:
	const void* m_pRedirectFrom {};
	const void* m_pRedirectTo {};
	bool m_bOnceDisabled {};
};

namespace PGHooker
{
	extern std::list<C_CallbackInfo> cCallbacksInfo;
	extern std::list<C_HookInfo> cHooksInfo;
	extern PVOID pVEHHandle;

	void GuardPage(const void *pAddress);

	void UnGuardPage(const void *pAddress);

	LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo);

	void Initialize();

	void Uninitilize();

	void CreateHook(const void *pFrom, const void *pTo);

	void DisableHookForOnce(const void *pFrom);

	void RemoveHook(const void *pFrom);

	void CreateCallback(const void *pAddress, int iFlags, PGHCALLBACK pCallback);

	void RemoveCallback(const void *pAddress);
}