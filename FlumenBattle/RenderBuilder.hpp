#pragma once

class RenderBuilder
{
	static RenderBuilder* instance_;

	RenderBuilder();

public:
	static void Initialize();
};
