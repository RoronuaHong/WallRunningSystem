// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WRCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class WALLRUNNINGSSYSTEM_API AWRCharacter: public ACharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	FHitResult OutHit;
	FHitResult OutLeftHit;
	FHitResult OutRightHit;
	FCollisionQueryParams TraceParams;
	AWRCharacter* WRCharacter;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void EnableWallRunning();
	void DisableWallRunning();

	void BeginJump();
	void endJump();

	void PlayerIsRunning();
	void HitResultImp();
	void ResetWallRunning();

	UFUNCTION(BlueprintCallable, Category = "WallRunningFunction")
	void EnableWallRun();

	bool bIsInAir;
	bool bIsIgnoreInput;
	bool bIsWallRunning;
	bool bEnableWallRunning;
	bool bIsWallRunningDone;
	bool bIsCorrentAngle;
	bool bIsDoOnce;

	bool bIsWallLeftHit;
	bool bIsWallRightHit;

	FVector NormalOfSurface;

	float MaxHeight;
	float ZValue;
	float Length;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetWallRunning() {
		return bIsWallRunning;
	}

	bool GetWallRight() {
		return bIsWallRightHit;
	}

	bool GetWallLeft() {
		return bIsWallLeftHit;
	}


};
