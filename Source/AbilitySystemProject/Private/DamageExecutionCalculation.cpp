// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageExecutionCalculation.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttributeSetBase.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetBase, AttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetBase, Armor, Target, true);
	}
};

static FDamageStatics& GetDamageStatics()
{
	static FDamageStatics DamageStaticsVar;
	return DamageStaticsVar;
}

UDamageExecutionCalculation::UDamageExecutionCalculation()
{
	HealthProperty = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Health));
	HealthDef = FGameplayEffectAttributeCaptureDefinition(HealthProperty, EGameplayEffectAttributeCaptureSource::Target, true);

	RelevantAttributesToCapture.Add(HealthDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
}

void UDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float AttackDamageMagnitude = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().AttackDamageDef, FAggregatorEvaluateParameters(), AttackDamageMagnitude);
	
	float ArmorMagnitude = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorDef, FAggregatorEvaluateParameters(), ArmorMagnitude);

	float FinalDamage = FMath::Clamp(AttackDamageMagnitude - ArmorMagnitude, 0.f, AttackDamageMagnitude - ArmorMagnitude);
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthProperty, EGameplayModOp::Additive, -FinalDamage));
}
