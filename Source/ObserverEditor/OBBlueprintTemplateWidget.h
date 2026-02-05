#pragma once
#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "OBBlueprintTemplateWidget.generated.h"

UCLASS()
class OBSERVEREDITOR_API UOBBlueprintTemplateWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|EditorUtils")
	bool SaveBlueprintClipboardToTemplate(FString TemplateName);
	
	UFUNCTION(BlueprintCallable, Category = "Observer|EditorUtils")
	FString LoadClipboardFromTemplate(FString TemplateName);
	
private:

	bool IsValidBlueprintClipboardContent(const FString& ClipboardContent);

	TMap<FString, FString> TemplateStorage;
};