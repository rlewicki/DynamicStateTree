// Copyright 2026, Robert Lewicki, All rights reserved.

#pragma once

#include "InstancedPropertyBagWrapper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateTreeReference.h"
#include "DynamicStateTreeFunctionLibrary.generated.h"

class UStateTree;

UCLASS()
class DYNAMICSTATETREE_API UDynamicStateTreeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void CreateDynamicStateTree(UStateTree* StateTree, FInstancedPropertyBagWrapper PropertyBagWrapper, FStateTreeReference& NewStateTreeRef);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetFloatValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, float Value);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetIntValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetBoolValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetNameValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FName Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetStringValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetTextValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FText Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetObjectValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, UObject* Value, UClass* ObjectClass);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetClassValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, TSubclassOf<UObject> Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static void SetByteValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, meta = (CustomStructureParam = "Value", AutoCreateRefTerm = "Value"))
	static void SetStructValue(UPARAM(Ref) FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, const int32& Value);
	
	DECLARE_FUNCTION(execSetStructValue);
private:
	static void InternalSetStructValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FConstStructView Value);
};
