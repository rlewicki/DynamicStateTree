// Copyright 2026, Robert Lewicki, All rights reserved.

#include "K2Node_CreateDynamicStateTree.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "StateTree.h"
#include "StateTreeReference.h"
#include "DynamicStateTreeFunctionLibrary.h"
#include "InstancedPropertyBagWrapper.h"
#include "StructUtils/PropertyBag.h"

namespace K2Node_CreateDynamicStateTree::Name
{
	static FName StateTreePin = "StateTree";
}

void UK2Node_CreateDynamicStateTree::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* StateTreePin = CreatePin(
		EGPD_Input, 
		UEdGraphSchema_K2::PC_Object, 
		UStateTree::StaticClass(), 
		K2Node_CreateDynamicStateTree::Name::StateTreePin);
	StateTreePin->bNotConnectable = true;
	if (StateTree)
	{
		if (const UPropertyBag* ParametersBag = StateTree->GetDefaultParameters().GetPropertyBagStruct())
		{
			const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
			for (const FPropertyBagPropertyDesc& PropertyDesc : ParametersBag->GetPropertyDescs())
			{
				UEdGraphPin* ParameterPin = CreatePin(EGPD_Input, NAME_None, PropertyDesc.Name);
				K2Schema->ConvertPropertyToPinType(PropertyDesc.CachedProperty, ParameterPin->PinType);
			}
		}
	}

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	FCreatePinParams Params;
	Params.bIsReference = true;
	CreatePin(
		EGPD_Output,
		UEdGraphSchema_K2::PC_Struct,
		FStateTreeReference::StaticStruct(),
		UEdGraphSchema_K2::PN_ReturnValue,
		Params);
}

FText UK2Node_CreateDynamicStateTree::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return INVTEXT("Create State Tree (Dynamic)");
}

FText UK2Node_CreateDynamicStateTree::GetMenuCategory() const
{
	return INVTEXT("State Tree");
}

void UK2Node_CreateDynamicStateTree::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner);
		ActionRegistrar.AddBlueprintAction(ActionKey, Spawner);
	}
}

bool UK2Node_CreateDynamicStateTree::IsConnectionDisallowed(
	const UEdGraphPin* MyPin,
	const UEdGraphPin* OtherPin,
	FString& OutReason) const
{
	if (MyPin->GetName() == K2Node_CreateDynamicStateTree::Name::StateTreePin)
	{
		OutReason = "StateTree value must be set directly, cannot be wired with external pin";
		return true;
	}

	return false;
}

void UK2Node_CreateDynamicStateTree::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (Pin->GetName() == K2Node_CreateDynamicStateTree::Name::StateTreePin)
	{
		StateTree = Cast<UStateTree>(Pin->DefaultObject);
		ReconstructNode();
	}
}

void UK2Node_CreateDynamicStateTree::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static FName CreateStateTreeFunctionName = GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, CreateDynamicStateTree);
	UK2Node_CallFunction* CreateStateTreeNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CreateStateTreeNode->FunctionReference.SetExternalMember(
		CreateStateTreeFunctionName,
		UDynamicStateTreeFunctionLibrary::StaticClass());
	CreateStateTreeNode->AllocateDefaultPins();

	UK2Node_TemporaryVariable* PropertyBagVariableNode = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(
		this,
		SourceGraph);
	PropertyBagVariableNode->VariableType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PropertyBagVariableNode->VariableType.PinSubCategoryObject = FInstancedPropertyBagWrapper::StaticStruct();
	PropertyBagVariableNode->AllocateDefaultPins();
	
	UEdGraphPin* StateTreePin = CreateStateTreeNode->FindPin(TEXT("StateTree"));
	StateTreePin->DefaultObject = StateTree;
	
	TMap<FName, FName> TypeToSetterFunction
	{
		{UEdGraphSchema_K2::PC_Real, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetFloatValue)},
		{UEdGraphSchema_K2::PC_Boolean, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetBoolValue)},
		{UEdGraphSchema_K2::PC_Int, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetIntValue)},
		{UEdGraphSchema_K2::PC_Name, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetNameValue)},
		{UEdGraphSchema_K2::PC_String, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetStringValue)},
		{UEdGraphSchema_K2::PC_Text, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetTextValue)},
		{UEdGraphSchema_K2::PC_Object, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetObjectValue)},
		{UEdGraphSchema_K2::PC_Class, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetClassValue)},
		{UEdGraphSchema_K2::PC_Byte, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetByteValue)},
		{UEdGraphSchema_K2::PC_Struct, GET_FUNCTION_NAME_CHECKED(UDynamicStateTreeFunctionLibrary, SetStructValue)},
	};

	bool bSuccess = true;
	TArray<UK2Node_CallFunction*, TInlineAllocator<32>> SetterFunctions;
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	for (int32 Index = 0; Index < Pins.Num(); Index++)
	{
		UEdGraphPin* Pin = Pins[Index];
		if (Pin->Direction != EGPD_Input
		    || Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec
		    || Pin->GetName() == K2Node_CreateDynamicStateTree::Name::StateTreePin)
		{
			continue;
		}
		
		const FName& PinCategory = Pin->PinType.PinCategory;

		if (!TypeToSetterFunction.Contains(PinCategory))
		{
			bSuccess = false;
			CompilerContext.MessageLog.Error(TEXT("Missing setter function for type @@"), Pin);
			continue;
		}

		const FName SetterFunctionName = TypeToSetterFunction.FindChecked(PinCategory);
		UK2Node_CallFunction* SetterFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		SetterFunction->FunctionReference.SetExternalMember(
			SetterFunctionName,
			UDynamicStateTreeFunctionLibrary::StaticClass());
		SetterFunctions.Add(SetterFunction);
		SetterFunction->AllocateDefaultPins();
		UEdGraphPin* WrapperPin = SetterFunction->FindPinChecked(TEXT("PropertyBagWrapper"));
		UEdGraphPin* PropertyNamePin = SetterFunction->FindPinChecked(TEXT("PropertyName"));
		UEdGraphPin* ValuePin = SetterFunction->FindPinChecked(TEXT("Value"));

		PropertyNamePin->DefaultValue = Pin->GetName();
		bSuccess &= Schema->TryCreateConnection(WrapperPin, PropertyBagVariableNode->GetVariablePin());
		
		// In case of struct, we need to copy the pin's type as the intermediate call function node is created to accept
		// a wildcard, meaning it doesn't know the type of struct. We need to manually set the type here to ensure it
		// compiles fine.
		if (PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			ValuePin->PinType = Pin->PinType;
			ValuePin->PinType.bIsReference = true;
			ValuePin->DefaultValue = Pin->DefaultValue;
		}
		else if (PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			UEdGraphPin* ObjectClassPin = SetterFunction->FindPinChecked(TEXT("ObjectClass"));
			ObjectClassPin->DefaultObject = Pin->DefaultObject;
		}

		const FPinConnectionResponse Response = CompilerContext.CopyPinLinksToIntermediate(*Pin, *ValuePin);
		if (Response.IsFatal())
		{
			CompilerContext.MessageLog.Error(*Response.Message.ToString(), Pin, ValuePin);
		}
	}

	if (SetterFunctions.IsEmpty())
	{
		CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *CreateStateTreeNode->GetExecPin());
	}
	else
	{
		const FPinConnectionResponse ConnectionResponse = CompilerContext.MovePinLinksToIntermediate(
			*GetExecPin(),
			*SetterFunctions[0]->GetExecPin());
		if (ConnectionResponse.IsFatal())
		{
			CompilerContext.MessageLog.Error(*ConnectionResponse.Message.ToString(), SetterFunctions[0]);
		}

		for (int32 Index = 0; Index < SetterFunctions.Num() - 1; Index++)
		{
			bSuccess &= Schema->TryCreateConnection(
				SetterFunctions[Index]->GetThenPin(),
				SetterFunctions[Index + 1]->GetExecPin());
			if (!bSuccess)
			{
				CompilerContext.MessageLog.Error(
					TEXT("Failed to connect execution pins between @@ and @@"),
					SetterFunctions[Index],
					SetterFunctions[Index + 1]);
			}
		}
		
		bSuccess &= Schema->TryCreateConnection(SetterFunctions.Last()->GetThenPin(), CreateStateTreeNode->GetExecPin());
		if (!bSuccess)
		{
			CompilerContext.MessageLog.Error(
				TEXT("Failed to connect execution pins between @@ function and @@"),
				SetterFunctions.Last(),
				CreateStateTreeNode);
		}
	}

	{
		UEdGraphPin* WrapperRefPin = CreateStateTreeNode->FindPinChecked(TEXT("PropertyBagWrapper"));
		bSuccess &= Schema->TryCreateConnection(WrapperRefPin, PropertyBagVariableNode->GetVariablePin());
		if (!bSuccess)
		{
			CompilerContext.MessageLog.Error(TEXT("Failed to connect PropertyBagWrapper to the @@"), CreateStateTreeNode);
		}
	}

	{
		const FPinConnectionResponse ConnectionResponse = CompilerContext.MovePinLinksToIntermediate(
			*GetThenPin(),
			*CreateStateTreeNode->GetThenPin());
		if (ConnectionResponse.IsFatal())
		{
			CompilerContext.MessageLog.Error(*ConnectionResponse.Message.ToString(), this, CreateStateTreeNode);
		}
	}
	
	{
		UEdGraphPin* NewStateTreeRefPin = CreateStateTreeNode->FindPinChecked(TEXT("NewStateTreeRef"));
		UEdGraphPin* ReturnValuePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
		const FPinConnectionResponse ConnectionResponse = CompilerContext.MovePinLinksToIntermediate(
			*ReturnValuePin,
			*NewStateTreeRefPin);
		if (ConnectionResponse.IsFatal())
		{
			CompilerContext.MessageLog.Error(*ConnectionResponse.Message.ToString(), this);
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_CreateDynamicStateTree::PreloadRequiredAssets()
{
	Super::PreloadRequiredAssets();
	
	PreloadObject(StateTree);
}
