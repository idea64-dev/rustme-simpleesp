#include "PGHooker.hpp"

std::list<C_CallbackInfo> PGHooker::cCallbacksInfo{};
std::list<C_HookInfo>     PGHooker::cHooksInfo{};
PVOID                    PGHooker::pVEHHandle{};

void PGHooker::GuardPage(const void *pAddress)
{
	DWORD oldProtect{};
	MEMORY_BASIC_INFORMATION mbi{};

	VirtualQuery(pAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	VirtualProtect((LPVOID)pAddress, 1, mbi.Protect | PAGE_GUARD, &oldProtect);
}

void PGHooker::UnGuardPage(const void *pAddress)
{
	DWORD oldProtect{};
	MEMORY_BASIC_INFORMATION mbi{};

	VirtualQuery(pAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	VirtualProtect((LPVOID)pAddress, 1, mbi.Protect & ~PAGE_GUARD, &oldProtect);
}

LONG WINAPI PGHooker::VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	static const void *pLastAddress = nullptr;

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
	{
		auto uType = pExceptionInfo->ExceptionRecord->ExceptionInformation[0];
		const void *pAddress = (const void *)pExceptionInfo->ExceptionRecord->ExceptionInformation[1];

		pLastAddress = pAddress;

		if (uType == EIZ_READ || uType == EIZ_WRITE)
		{
			for (auto &cInfo : cCallbacksInfo)
			{
				if (pAddress != cInfo.m_pAddress)
					continue;

				E_CallbackFlags eExFlag = E_CallbackFlags(1 << uType);

				if (!(cInfo.m_eFlags & eExFlag))
					continue;

				cInfo.m_pCallback(pExceptionInfo->ContextRecord, eExFlag);
			}
		}
		else if (uType == EIZ_DEPVIO)
		{
			for (auto &cInfo : cHooksInfo)
			{
				if (pAddress != cInfo.m_pRedirectFrom)
					continue;

				if (cInfo.m_bOnceDisabled)
				{
					cInfo.m_bOnceDisabled = false;
					continue;
				}
#ifdef _WIN64
				pExceptionInfo->ContextRecord->Rip = (DWORD64)cInfo.m_pRedirectTo;
#else
				pExceptionInfo->ContextRecord->Eip = (DWORD)cInfo.m_pRedirectTo;
#endif
				break;
			}
		}

		pExceptionInfo->ContextRecord->EFlags |= 0x100ui32;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		if (pLastAddress)
		{
			GuardPage(pLastAddress);
			pLastAddress = nullptr;
		}

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void PGHooker::Initialize()
{
	pVEHHandle = AddVectoredExceptionHandler(1u, VectoredHandler);
}

void PGHooker::Uninitilize()
{
	for (auto &cInfo : cHooksInfo)
		UnGuardPage(cInfo.m_pRedirectFrom);

	cHooksInfo.clear();

	for (auto &cInfo : cCallbacksInfo)
		UnGuardPage(cInfo.m_pAddress);

	cCallbacksInfo.clear();

	RemoveVectoredExceptionHandler(pVEHHandle);
}

void PGHooker::CreateHook(const void *pFrom, const void *pTo)
{
	bool bGuard = true;

	for (auto &cInfo : cHooksInfo)
	{
		if (cInfo.m_pRedirectFrom == pFrom)
			return;

		if (GetPage(cInfo.m_pRedirectFrom) == GetPage(pFrom))
			bGuard = false;
	}

	cHooksInfo.push_back({ pFrom, pTo, false });

	if (bGuard)
		GuardPage(pFrom);
}

void PGHooker::DisableHookForOnce(const void *pFrom)
{
	for (auto &cInfo : cHooksInfo)
	{
		if (cInfo.m_pRedirectFrom != pFrom)
			continue;

		cInfo.m_bOnceDisabled = true;
		break;
	}
}

void PGHooker::RemoveHook(const void *pFrom)
{
	std::list < C_HookInfo >::iterator i = cHooksInfo.begin();
	bool bRemoved = false;

	while (i != cHooksInfo.end())
	{
		if (i->m_pRedirectFrom == pFrom)
		{
			cHooksInfo.erase(i);
			bRemoved = true;
			break;
		}

		++i;
	}

	if (!bRemoved)
		return;

	bool bUnGuard = true;

	for (auto &cInfo : cHooksInfo)
	{
		if (GetPage(cInfo.m_pRedirectFrom) == GetPage(pFrom))
		{
			bUnGuard = false;
			break;
		}
	}

	if (bUnGuard)
		UnGuardPage(pFrom);
}

void PGHooker::CreateCallback(const void *pAddress, int iFlags, PGHCALLBACK pCallback)
{
	bool bGuard = true;

	for (auto &cInfo : cCallbacksInfo)
	{
		if (GetPage(cInfo.m_pAddress) == GetPage(pAddress))
		{
			bGuard = false;
			break;
		}
	}

	cCallbacksInfo.push_back({ pAddress, pCallback, (E_CallbackFlags)iFlags });

	if (bGuard)
		GuardPage(pAddress);
}

void PGHooker::RemoveCallback(const void *pAddress)
{
	std::list < C_CallbackInfo >::iterator i = cCallbacksInfo.begin();
	bool bRemoved = false;

	while (i != cCallbacksInfo.end())
	{
		if (i->m_pAddress == pAddress)
		{
			i = cCallbacksInfo.erase(i);
			bRemoved = true;
		}
		else
			++i;
	}

	if (!bRemoved)
		return;

	bool bUnGuard = true;

	for (auto &cInfo : cCallbacksInfo)
	{
		if (GetPage(cInfo.m_pAddress) == GetPage(pAddress))
		{
			bUnGuard = false;
			break;
		}
	}

	if (bUnGuard)
		UnGuardPage(pAddress);
}