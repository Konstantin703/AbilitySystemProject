#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilityTypes.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEMPROJECT_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerControllerBase")
	void AddAbilityToUI(FGameplayAbilityInfo AbilityInfo);	
	
};
