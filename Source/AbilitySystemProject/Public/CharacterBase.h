// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilityTargetTypes.h"
#include "CharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSetBase;
class UGameplayAbilityBase;

UCLASS()
class ABILITYSYSTEMPROJECT_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	USpringArmComponent* CameraBoom;
	UPROPERTY()
	UCameraComponent* PlayerEye;
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAbilitySystemComponent* AbilitySystemComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAttributeSetBase* AttributeSetBaseComp;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AquireAbility(TSubclassOf<UGameplayAbility> AbilityToAquire);
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AquireAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToAquire);

	UFUNCTION()
	void OnHealthChanged(float Health, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnHealthChanged"))
	void BP_OnHealthChanged(float Health, float MaxHealth);
	
	UFUNCTION()
	void OnManaChanged(float Mana, float MaxMana);
	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnManaChanged"))
	void BP_OnManaChanged(float Mana, float MaxMana);

	UFUNCTION()
	void OnStrengthChanged(float Strength, float MaxStrength);
	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnStrengthChanged"))
	void BP_OnStrengthChanged(float Strength, float MaxStrength);
	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "Die"))
	void BP_Die();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	bool IsOtherHostile(ACharacterBase* InPawn);

	uint8 GetTeamID() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AddGameplayTag(FGameplayTag& TagToAdd);
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void RemoveGameplayTag(FGameplayTag& TagToRemove);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	FGameplayTag FullHealthTag;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void HitStun(float StunDuration);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void ApplyGameplayEffectHandle(const FGameplayEffectSpecHandle& SpecHandle, const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:
	bool bIsDead;
	uint8 TeamID;

	void AutoDetermineTeamIDByControllerType();
	void Dead();
	void DisableInputControl();
	void EnableInputControl();

	FTimerHandle StunTimeHandle;

	void AddAbilityToUI(TSubclassOf<UGameplayAbilityBase> AbilityToAdd);
};
