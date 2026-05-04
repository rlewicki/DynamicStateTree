// Copyright 2026, Robert Lewicki, All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FDynamicStateTreeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
