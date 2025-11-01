#include "rule.h"

bool Rule::Check(const Context& context) {
	return this->Filter(context);
}

void Rule::Execute(Context& context) {
    if (this->Check(context)) {
        this->Callback(context);
    }
}