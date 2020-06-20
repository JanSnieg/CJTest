// Fill out your copyright notice in the Description page of Project Settings.


#include "GFTUserWidget.h"
#include "HeavyObjectCharacter.h"
#include "Components/TextBlock.h"
#include "GFTComponent.h"

UGFTUserWidget::UGFTUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Quantity = CreateDefaultSubobject<UTextBlock>(FName("Quantity"));
    Weight = CreateDefaultSubobject<UTextBlock>(FName("Weight"));
}

void UGFTUserWidget::UpdateCount(UGFTComponent* GFT)
{
    if (GFT->GetNumberOfLinks() > 0 || GFT->GetTotalMass() > 0)
    {
        const FText QuantityText = FText::FromString( FString("Quantity: \n") + FString::FromInt(GFT->GetNumberOfLinks()) + FString(" / ") + FString::FromInt(GFT->MaxLinks));
        const FText WeightText = FText::FromString( FString("Mass: \n") + FString::SanitizeFloat(GFT->GetTotalMass(), 0) + FString(" / ") + FString::SanitizeFloat(GFT->MaxLift, 0));
        Quantity->SetText(QuantityText);
        Weight->SetText(WeightText);

        if ((Weight->Visibility == ESlateVisibility::Hidden || Quantity->Visibility == ESlateVisibility::Hidden) && FadeAnimation)
        {
            PlayAnimation(FadeAnimation);
            Weight->SetVisibility(ESlateVisibility::Visible);
            Quantity->SetVisibility(ESlateVisibility::Visible);
        }

        if (UpdateAnimation)
        {
            PlayAnimation(UpdateAnimation);
        }
    }
    else
    {
        Weight->SetVisibility(ESlateVisibility::Hidden);
        Quantity->SetVisibility(ESlateVisibility::Hidden);
    }
}

UWidgetAnimation* UGFTUserWidget::GetAnimationByName(const FName AnimationName)
{
    UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);
    if (WidgetAnimation)
    {
        return *WidgetAnimation;
    }
    return nullptr;
}

void UGFTUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StoreWidgetAnimations();
    UpdateAnimation = GetAnimationByName(TEXT("Update"));
    FadeAnimation = GetAnimationByName(TEXT("Fade"));
}

void UGFTUserWidget::NativeOnInitialized()
{
    if (AHeavyObjectCharacter* Character = GetOwningPlayerPawn<AHeavyObjectCharacter>())
    {
        if (UGFTComponent* GFT = Cast<UGFTComponent>(Character->GetComponentByClass(UGFTComponent::StaticClass())))
        {
            GFT->OnUpdated.AddUniqueDynamic(this, &UGFTUserWidget::UpdateCount);
        }
    }
}

void UGFTUserWidget::StoreWidgetAnimations()
{
    AnimationsMap.Empty();
    
    UProperty* Property = GetClass()->PropertyLink;
    while (Property)
    {
        //Only take object properties
        if (Property->GetClass() == UObjectProperty::StaticClass())
        {
            UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property);

            //Only take widget Animations properties
            if (ObjectProperty->PropertyClass == UWidgetAnimation::StaticClass())
            {
                UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(this);
                UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Object);

                if(WidgetAnimation && WidgetAnimation->MovieScene)
                {
                    FName AnimationName = WidgetAnimation->MovieScene->GetFName();
                    UE_LOG(LogAnimation, Log, TEXT("Storing %s Animation to %s"), *AnimationName.ToString(), *GetName());
                    AnimationsMap.Add(AnimationName, WidgetAnimation);
                }
            }
        }
        Property = Property->PropertyLinkNext;
    }
}
