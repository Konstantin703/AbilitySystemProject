// Fill out your copyright notice in the Description page of Project Settings.

#include "GATargetActorGroundSelect.h"
#include "GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

AGATargetActorGroundSelect::AGATargetActorGroundSelect()
{
	PrimaryActorTick.bCanEverTick = true;

	RootDummy = CreateDefaultSubobject<USceneComponent>("RootDummy");
	SetRootComponent(RootDummy);
	
	Radius = 200.f;
	
	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->SetupAttachment(RootDummy);
	Decal->DecalSize = FVector(Radius);
}

void AGATargetActorGroundSelect::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
	Decal->DecalSize = FVector(Radius);
}

void AGATargetActorGroundSelect::ConfirmTargetingAndContinue()
{
	FVector ViewLocation;
	GetPlayerLookPoint(ViewLocation);

	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>> OverlapedActors;

	bool bTraceComplex = false;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = bTraceComplex;
	CollisionQueryParams.bReturnPhysicalMaterial = false;

	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		CollisionQueryParams.AddIgnoredActor(MasterPC->GetUniqueID());
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

void AGATargetActorGroundSelect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector LookPoint;
	GetPlayerLookPoint(LookPoint);
	Decal->SetWorldLocation(LookPoint);
}

bool AGATargetActorGroundSelect::GetPlayerLookPoint(FVector& OutViewPoint)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	MasterPC->GetPlayerViewPoint(ViewPoint, ViewRotation);
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	
	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		QueryParams.AddIgnoredActor(MasterPC->GetUniqueID());
	}

	bool bTrace = GetWorld()->LineTraceSingleByChannel(
		HitResult, ViewPoint, ViewPoint + ViewRotation.Vector() * 10000.f, ECC_Visibility, QueryParams
	);
	bTrace ? OutViewPoint = HitResult.ImpactPoint : OutViewPoint = FVector();

	return bTrace;
}

