#pragma once

enum class Drx : int {
	Dr0,
	Dr1,
	Dr2,
	Dr3,
	Invalid
};

enum class WatchpointLength : unsigned long long {
	OneByte = 0x0,
	TwoBytes = 0x1,
	FourBytes = 0x3,
	EightBytes = 0x2,
};

enum class WatchpointCondition : unsigned long long {
	Execution = 0b00,
	Write = 0b01,
	Undefined = 0b10,
	ReadWrite = 0b11,
};

enum class EngineResult : int {
	Success,
	Failure,
};