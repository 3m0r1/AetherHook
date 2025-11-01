#pragma once

#include<windows.h>
#include<DbgHelp.h>
#include<Psapi.h>

#include<vector>
#include<memory>

#include "types.h"

class Hook;

class Context {
public:
	PCONTEXT ContextRecord;
	PEXCEPTION_RECORD ExceptionRecord;
	std::shared_ptr<Hook> EngineHook;

	static Context BuildContext(PEXCEPTION_POINTERS exception, std::shared_ptr<Hook> hook);

	void Return();

	template<typename T>
	T GetArg(int index) const {
		
		if (index == 0) return (T)this->ContextRecord->Rcx;
		if (index == 1) return (T)this->ContextRecord->Rdx;
		if (index == 2) return (T)this->ContextRecord->R8;
		if (index == 3) return (T)this->ContextRecord->R9;

		if (index > 3) {
			return *(T*)( this->ContextRecord->Rsp + ( (index + 1) * 0x8) );
		}

		return T{};
	}


	template<typename T>
	void SetArg(int index, T value) {

		if (index > 3) {
			*(T*)(this->ContextRecord->Rsp + ((index + 1) * 0x8)) = value;
		}

		if (index == 0) this->ContextRecord->Rcx = (DWORD64)value;
		if (index == 1) this->ContextRecord->Rdx = (DWORD64)value;
		if (index == 2) this->ContextRecord->R8 = (DWORD64)value;
		if (index == 3) this->ContextRecord->R9 = (DWORD64)value;
	}

	template <typename T>
	void SetReturnValue(T value) {
		this->ContextRecord->Rax = (DWORD64)value;
	}

	template <typename T>
	void ReturnWith(T value) {
		this->SetReturnValue(value);
		this->Return();
	}

	DWORD64 GetIP() const;
	
	bool CalledFrom(HMODULE moduleHandle) const;

	void ActivateHook();
	void DeactivateHook();
};