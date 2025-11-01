#include "hook.h"
#include "rule.h"
#include "engine.h"

void Hook::AddRule(Rule rule) {
	this->Rules.push_back(std::move(rule));
}

void Hook::SetActive() {
	if (this->Active)
		return;

	this->Active = true;
}

void Hook::SetInactive() {
	if (!this->Active)
		return;

	this->Active = false;
}

void ExecutionHook::Activate() {
	this->SetActive();
	
	if (this->Global) {
		Engine::SetBreakPointGlobal(
			this->Address,
			static_cast<Drx>(this->Index)
		);
	}
	else {
		Engine::SetBreakPoint(
			GetCurrentThread(),
			this->Address,
			static_cast<Drx>(this->Index)
		);
	}
}

void ExecutionHook::Deactivate() {
	this->SetInactive();

	if (this->Global) {
		Engine::RemoveBreakPointGlobal(
			static_cast<Drx>(this->Index)
		);
	}
	else {
		Engine::RemoveBreakPoint(
			GetCurrentThread(),
			static_cast<Drx>(this->Index)
		);
	}
}

void WatchpointHook::Activate() {
	this->SetActive();

	if (this->Global) {
		Engine::SetWatchPointGlobal(
			this->Address,
			static_cast<Drx>(this->Index),
			this->Condition,
			this->Length
		);

	}
	else {
		Engine::SetWatchPoint(
			GetCurrentThread(),
			this->Address,
			static_cast<Drx>(this->Index),
			this->Condition,
			this->Length
		);
	}
}

void WatchpointHook::Deactivate() {
	this->SetInactive();

	if (this->Global) {
		Engine::RemoveWatchPointGlobal(
			static_cast<Drx>(this->Index)
		);
	}
	else {
		Engine::RemoveWatchPoint(
			GetCurrentThread(),
			static_cast<Drx>(this->Index)
		);
	}
}