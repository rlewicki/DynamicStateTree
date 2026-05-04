// Copyright 2026, Robert Lewicki, All rights reserved.

#pragma once

#include "StructUtils/PropertyBag.h"

#include "InstancedPropertyBagWrapper.generated.h"

USTRUCT(BlueprintType, meta=(HiddenByDefault, BlueprintInternalUseOnly))
struct DYNAMICSTATETREE_API FInstancedPropertyBagWrapper
{
	GENERATED_BODY()
	
	UPROPERTY()
	FInstancedPropertyBag PropertyBag;
};
