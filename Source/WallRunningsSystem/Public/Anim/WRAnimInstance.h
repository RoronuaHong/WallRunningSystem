// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WRAnimInstance.generated.h"

class AActor;
class AWRCharacter;
/**
 * 
 */
UCLASS()
class WALLRUNNINGSSYSTEM_API UWRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	AActor* OwningActor;
	AWRCharacter* MyCharacter;

	void BlueprintBeginPlay();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UWRAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsWallRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsWallRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsWallLeft;
};
