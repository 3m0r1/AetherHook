#pragma once

#include<functional>
#include<vector>
#include<string>

#include "context.h"

class Rule {
private:
	std::function<bool(const Context& context)> Filter;
	std::function<void(Context& context)> Callback;

public:
	Rule(std::function<bool(const Context& context)> filter, std::function<void(Context& context)> callback) : Filter(std::move(filter)), Callback(std::move(callback)) {};

	bool Check(const Context& context);
	void Execute(Context& context);
};