// Fill out your copyright notice in the Description page of Project Settings.

#include "GATargetActorGetAround.h"
#include "GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void AGATargetActorGetAround::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
}

void AGATargetActorGetAround::ConfirmTargetingAndContinue()
{
	APawn* OwningPawn = MasterPC->GetPawn();
	if (!OwningPawn)
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
		return;
	}
	FVector ViewLocation = OwningPawn->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>> OverlapedActors;

	bool bTraceComplex = false;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = bTraceComplex;
	CollisionQueryParams.bReturnPhysicalMaterial = false;

	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		CollisionQueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}

	bool bTryOverlap = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ViewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius),
		CollisionQueryParams
	);

	if (bTryOverlap)
	{
		for (int32 i = 0; i < Overlaps.Num(); i++)
		{
			APawn* PawnOverlaped = Cast<APawn>(Overlaps[i].GetActor());
			if (PawnOverlaped && !OverlapedActors.Contains(PawnOverlaped))
			{
				OverlapedActors.Add(PawnOverlaped);
			}
		}
	}

	if (OverlapedActors.Num())
	{
		FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlapedActors);

		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
}

