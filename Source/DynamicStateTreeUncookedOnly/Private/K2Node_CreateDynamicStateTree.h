// Copyright 2026, Robert Lewicki, All rights reserved.

#pragma once

#include "K2Node.h"

#include "K2Node_CreateDynamicStateTree.generated.h"

class UStateTree;

UCLASS()
class UK2Node_CreateDynamicStateTree : public UK2Node
{
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
private:
	UPROPERTY()
	TObjectPtr<UStateTree> StateTree;
};
