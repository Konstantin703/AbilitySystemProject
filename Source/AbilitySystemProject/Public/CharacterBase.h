// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySystemComponent;
class UGameplayAbility;

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
	/*void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAbilitySystemComponent* AbilitySystemComp;

	//virtual UAbilitySystemComponent* GetAbilitySystemComponent() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AquireAbility(TSubclassOf<UGameplayAbility> AbilityToAquire);

	
};
