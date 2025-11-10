// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "StatsGauge.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;
class UImage;
class UTexture2D;
/**
 * 属性的UI设计
 */
UCLASS()
class UStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttributeText;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere, Category = "Visual")
	TObjectPtr<UTexture2D> IconTexture;

	void SetValue(float NewValue);

	FNumberFormattingOptions NumberFormattnigOptions;

	void AttributeChanged(const FOnAttributeChangeData& Data);
	
};
