// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilityBase.h"


FGameplayAbilityInfo UGameplayAbilityBase::GetAbilityInfo()
{
	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (CooldownEffect && CostEffect)
	{
		float CooldownDuration = 0.f;
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1.f, CooldownDuration);
		float Cost = 0.f;
		EAbilityCostType CostType;

		if (CostEffect->Modifiers.Num() > 0)
		{
			FGameplayModifierInfo EffectInfo = CostEffect->Modifiers[0];
			EffectInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1.f, Cost);
			FGameplayAttribute CostAttribute = EffectInfo.Attribute;
			FString AttributeName = CostAttribute.AttributeName;

			if (AttributeName == "Health")
			{
				CostType = EAbilityCostType::Health;
			}
			else if (AttributeName == "Mana")
			{
				CostType = EAbilityCostType::Mana;
			}
			else if (AttributeName == "Strength")
			{
				CostType = EAbilityCostType::Strength;
			}
		}
		return FGameplayAbilityInfo(CooldownDuration, Cost, CostType, UIMaterial, GetClass());
	}
	return FGameplayAbilityInfo();
}


