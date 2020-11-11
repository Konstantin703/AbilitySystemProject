#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->bUsePawnControlRotation = true;

	PlayerEye = CreateDefaultSubobject<UCameraComponent>("PlayerEye");
	PlayerEye->SetupAttachment(CameraBoom);

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSetBaseComp = CreateDefaultSubobject<UAttributeSetBase>("AttributeSetBaseComponent");

	bIsDead = false;
	TeamID = 255;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	AttributeSetBaseComp->OnHealthChange.AddDynamic(this, &ACharacterBase::OnHealthChanged);
	AttributeSetBaseComp->OnManaChange.AddDynamic(this, &ACharacterBase::OnManaChanged);
	AttributeSetBaseComp->OnStrengthChange.AddDynamic(this, &ACharacterBase::OnStrengthChanged);
	AutoDetermineTeamIDByControllerType(); 
	AddGameplayTag(FullHealthTag);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("LookingRight", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookingUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

}

void ACharacterBase::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(UGameplayStatics::GetPlayerCameraManager(this, 0)->GetActorForwardVector(), Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(UGameplayStatics::GetPlayerCameraManager(this, 0)->GetActorRightVector(), Value);
	}
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void ACharacterBase::AquireAbility(TSubclassOf<UGameplayAbility> AbilityToAquire)
{
	if (AbilitySystemComp) 
	{
		if (HasAuthority() && AbilityToAquire)
		{
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(AbilityToAquire, 1, 0));
		}
		AbilitySystemComp->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::OnHealthChanged(float Health, float MaxHealth)
{
	if (Health <= 0.f && !bIsDead)
	{
		bIsDead = true;
		Dead();
		BP_Die();
	}
	BP_OnHealthChanged(Health, MaxHealth);
}

void ACharacterBase::OnManaChanged(float Mana, float MaxMana)
{
	BP_OnManaChanged(Mana, MaxMana);
}

void ACharacterBase::OnStrengthChanged(float Strength, float MaxStrength)
{
	BP_OnStrengthChanged(Strength, MaxStrength);
}

bool ACharacterBase::IsOtherHostile(ACharacterBase* InPawn)
{
	return TeamID != InPawn->GetTeamID();
}

uint8 ACharacterBase::GetTeamID() const
{
	return TeamID;
}

void ACharacterBase::AddGameplayTag(FGameplayTag& TagToAdd)
{
	GetAbilitySystemComponent()->AddLooseGameplayTag(TagToAdd);
	GetAbilitySystemComponent()->SetTagMapCount(TagToAdd, 1);
}

void ACharacterBase::RemoveGameplayTag(FGameplayTag& TagToRemove)
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(TagToRemove);
}

void ACharacterBase::AutoDetermineTeamIDByControllerType()
{
	if (GetController() && GetController()->IsPlayerController())
	{
		TeamID = 0;
	}
}

void ACharacterBase::Dead()
{
	DisableInputControl();
}

void ACharacterBase::DisableInputControl()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->DisableInput(PlayerController);
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
	}
}

void ACharacterBase::EnableInputControl()
{
	if (!bIsDead)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->EnableInput(PlayerController);
		}

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->GetBrainComponent()->RestartLogic();
		}
	}	
}

void ACharacterBase::HitStun(float StunDuration)
{
	DisableInputControl();

	GetWorldTimerManager().SetTimer(StunTimeHandle, this, &ACharacterBase::EnableInputControl, StunDuration, false);

}
