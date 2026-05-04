// Copyright 2026, Robert Lewicki, All rights reserved.

#include "DynamicStateTreeFunctionLibrary.h"

#include "StateTreeReference.h"
#include "StateTree.h"
#include "Blueprint/BlueprintExceptionInfo.h"

namespace DynamicStateTree::Private
{
void CopyPropertyBagValuesByName(const FInstancedPropertyBag& Source, FInstancedPropertyBag& Target)
{
	const UPropertyBag* SourceBagDef = Source.GetPropertyBagStruct();
	const UPropertyBag* TargetBagDef = Target.GetPropertyBagStruct();

	if (!SourceBagDef || !TargetBagDef)
	{
		return;
	}

	const uint8* SourceMemory = Source.GetValue().GetMemory();
	uint8* TargetMemory = Target.GetMutableValue().GetMemory();

	if (!SourceMemory || !TargetMemory)
	{
		return;
	}

	for (const FPropertyBagPropertyDesc& SourceDesc : SourceBagDef->GetPropertyDescs())
	{
		const FPropertyBagPropertyDesc* TargetDesc = TargetBagDef->FindPropertyDescByName(SourceDesc.Name);
		if (!TargetDesc)
		{
			continue;
		}

		const FProperty* SourceProp = SourceDesc.CachedProperty;
		const FProperty* TargetProp = TargetDesc->CachedProperty;
		if (SourceProp && TargetProp && SourceProp->SameType(TargetProp))
		{
			TargetProp->CopyCompleteValue(TargetProp->ContainerPtrToValuePtr<void>(TargetMemory), SourceProp->ContainerPtrToValuePtr<void>(SourceMemory));
		}
	}
}
}

void UDynamicStateTreeFunctionLibrary::CreateDynamicStateTree(UStateTree* StateTree, FInstancedPropertyBagWrapper PropertyBagWrapper, FStateTreeReference& NewStateTreeRef)
{
	NewStateTreeRef.SetStateTree(StateTree);
	NewStateTreeRef.SyncParameters();
	
	if (PropertyBagWrapper.PropertyBag.GetPropertyBagStruct())
	{
		for (const FPropertyBagPropertyDesc& PropertyDesc : NewStateTreeRef.GetMutableParameters().GetPropertyBagStruct()->GetPropertyDescs())
		{
			NewStateTreeRef.SetPropertyOverridden(PropertyDesc.ID, true);
		}

		DynamicStateTree::Private::CopyPropertyBagValuesByName(PropertyBagWrapper.PropertyBag, NewStateTreeRef.GetMutableParameters());
	}
}

void UDynamicStateTreeFunctionLibrary::SetFloatValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, float Value)
{
	TValueOrError<float, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueFloat(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Float);
	}
	PropertyBagWrapper.PropertyBag.SetValueFloat(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetIntValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, int32 Value)
{
	TValueOrError<int32, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueInt32(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Int32);
	}
	PropertyBagWrapper.PropertyBag.SetValueInt32(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetBoolValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, bool Value)
{
	TValueOrError<bool, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueBool(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Bool);
	}
	PropertyBagWrapper.PropertyBag.SetValueBool(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetNameValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FName Value)
{
	TValueOrError<FName, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueName(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Name);
	}
	PropertyBagWrapper.PropertyBag.SetValueName(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetStringValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, const FString& Value)
{
	TValueOrError<FString, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueString(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::String);
	}
	PropertyBagWrapper.PropertyBag.SetValueString(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetTextValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FText Value)
{
	TValueOrError<FText, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueText(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Text);
	}
	PropertyBagWrapper.PropertyBag.SetValueText(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetObjectValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, UObject* Value)
{
	TValueOrError<UObject*, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueObject(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Object, Value->StaticClass());
	}
	PropertyBagWrapper.PropertyBag.SetValueObject(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetClassValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, TSubclassOf<UObject> Value)
{
	TValueOrError<UClass*, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueClass(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Class, Value->StaticClass());
	}
	PropertyBagWrapper.PropertyBag.SetValueClass(PropertyName, Value);
}

void UDynamicStateTreeFunctionLibrary::SetByteValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, int32 Value)
{
	TValueOrError<uint8, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueByte(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Byte);
	}
	PropertyBagWrapper.PropertyBag.SetValueByte(PropertyName, static_cast<uint8>(Value));
}

void UDynamicStateTreeFunctionLibrary::SetStructValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, const int32& Value)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UDynamicStateTreeFunctionLibrary::execSetStructValue)
{
	P_GET_STRUCT_REF(FInstancedPropertyBagWrapper, PropertyBagWrapper);
	P_GET_PROPERTY(FNameProperty, PropertyName);
	Stack.Step(Stack.Object, nullptr);
	const FStructProperty* ItemProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* ItemDataPtr = Stack.MostRecentPropertyAddress;
	P_FINISH
	
	if (!ItemProperty || !ItemDataPtr)
	{
		FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, INVTEXT("Failed to resolve the struct value for SetStructValue function"));
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	
	P_NATIVE_BEGIN
	if (ItemProperty && ItemDataPtr)
	{
		FConstStructView StructView(ItemProperty->Struct, static_cast<uint8*>(ItemDataPtr));
		InternalSetStructValue(PropertyBagWrapper, PropertyName, StructView);
	}
	P_NATIVE_END
}

void UDynamicStateTreeFunctionLibrary::InternalSetStructValue(FInstancedPropertyBagWrapper& PropertyBagWrapper, FName PropertyName, FConstStructView Value)
{
	TValueOrError<FStructView, EPropertyBagResult> Result = PropertyBagWrapper.PropertyBag.GetValueStruct(PropertyName);
	if (Result.HasError())
	{
		PropertyBagWrapper.PropertyBag.AddProperty(PropertyName, EPropertyBagPropertyType::Struct, Value.GetScriptStruct());
	}
	PropertyBagWrapper.PropertyBag.SetValueStruct(PropertyName, Value);
}
