// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/WRAnimInstance.h"
#include "Character/WRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UWRAnimInstance::UWRAnimInstance() {
	Speed = 0.0f;
	IsInAir = false;
	bIsWallRun = false;
	bIsWallRight = false;
	bIsWallLeft = false;
}


void UWRAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();
}

void UWRAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwningActor = GetOwningActor();

	if(OwningActor) {
		Speed = OwningActor->GetVelocity().Size();

		MyCharacter = Cast<AWRCharacter>(OwningActor);

		if(MyCharacter) {
			IsInAir = MyCharacter->GetCharacterMovement()->IsFalling();
			bIsWallRun = MyCharacter->GetWallRunning();
			bIsWallLeft = MyCharacter->GetWallLeft();
			bIsWallRight = MyCharacter->GetWallRight();
		}
	}
}