#pragma once

#include<vector>
#include<memory>

#include<windows.h>

#include "types.h"

#include "rule.h"

class Hook {
protected:
	void SetActive();
	void SetInactive();

public:
	PVOID Address;
	bool Global;
	bool Active;
	int Index;

	std::vector<Rule> Rules;

	Hook(PVOID address, bool global, int index) : Address(address), Active(true), Global(global), Index(index) {};

	void AddRule(Rule rule);

	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
};


class ExecutionHook : public Hook {
public:

	ExecutionHook(PVOID address, bool global, int index) : Hook(address, global, index) {};

	void Activate();
	void Deactivate();
};

class WatchpointHook : public Hook {
public:
	WatchpointLength Length;
	WatchpointCondition Condition;

	WatchpointHook(PVOID address, bool global, int index, WatchpointLength length, WatchpointCondition condition) : Hook(address, global, index), Length(length), Condition(condition) {};

	void Activate();
	void Deactivate();
};