// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWRCharacter::AWRCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	GetCharacterMovement()->GravityScale = 1.0f;

	bIsDoOnce = true;
	bIsWallRunning = false;
	bEnableWallRunning = false;
	bIsWallRunningDone = false;

	Length = 1.0f;
}

// Called when the game starts or when spawned
void AWRCharacter::BeginPlay() {
	Super::BeginPlay();

	WRCharacter = Cast<AWRCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

// Called every frame
void AWRCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	PlayerIsRunning();
}

void AWRCharacter::MoveForward(float Value) {
	if(!bIsIgnoreInput) {
		AddMovementInput(GetActorForwardVector() * Value);
	}
}

void AWRCharacter::MoveRight(float Value) {
	if(!bIsIgnoreInput) {
		AddMovementInput(GetActorRightVector() * Value);
	}
}

void AWRCharacter::BeginCrouch() {
	if(!bIsIgnoreInput) {	
		Crouch();

		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void AWRCharacter::EndCrouch() {
	if(!bIsIgnoreInput) {
		UnCrouch();
	}
}

void AWRCharacter::EnableWallRunning() {
	bEnableWallRunning = true;
}

void AWRCharacter::DisableWallRunning() {
	bEnableWallRunning = false;
	bIsWallRunningDone = true;
	bIsWallRightHit = false;
	bIsWallLeftHit = false;

	OutHit.Reset();
}

void AWRCharacter::BeginJump() {
	if(!bIsIgnoreInput) {
		if(bIsWallRunning) {
			bIsWallRunningDone = true;

			WRCharacter->SetActorRotation(UKismetMathLibrary::MakeRotFromX(NormalOfSurface));

			FVector CurrentNOS = NormalOfSurface * 1000.f;
			CurrentNOS.Z += 100.f;

			WRCharacter->LaunchCharacter(CurrentNOS, false, false);

			ResetWallRunning();
		} else {
			Jump();
		}
	}
}

void AWRCharacter::endJump() {
	if(!bIsWallRunning) {
		StopJumping();
	}
}

void AWRCharacter::PlayerIsRunning() {
	if(bIsWallRunning || (WRCharacter->GetVelocity()).Size() >= 500.f) {
		if(bEnableWallRunning) {
			if(bIsWallRunningDone) {
				ResetWallRunning();
			} else {
				bIsInAir = WRCharacter->GetCharacterMovement()->IsFalling();

				// Sequence 1:
				TraceParams.AddIgnoredActor(WRCharacter);

				FVector StartVector = WRCharacter->GetActorLocation();
				FVector EndVector = WRCharacter->GetActorLocation() + WRCharacter->GetActorRightVector() * 150.f;

				bIsWallRightHit = GetWorld()->LineTraceSingleByChannel(OutRightHit, StartVector, EndVector, ECC_GameTraceChannel1, TraceParams);
				//DrawDebugLine(GetWorld(), StartVector, EndVector, FColor::Red, false, -1.0f, '\000', 2.0f);

				EndVector = WRCharacter->GetActorLocation() + WRCharacter->GetActorRightVector() * 150.f * -1;

				bIsWallLeftHit = GetWorld()->LineTraceSingleByChannel(OutLeftHit, StartVector, EndVector, ECC_GameTraceChannel1, TraceParams);
				//DrawDebugLine(GetWorld(), StartVector, EndVector, FColor::Blue, false, -1.0f, '\000', 2.0f);

				if(bIsWallLeftHit && bIsWallRightHit) {
					if(OutLeftHit.Distance < OutRightHit.Distance) {
						OutHit = OutLeftHit;
					} else {
						OutHit = OutRightHit;
					}
				} else if(bIsWallLeftHit) {
					OutHit = OutLeftHit;
				} else if(bIsWallRightHit) {
					OutHit = OutRightHit;
				}
			}
		} else {
			if(bIsWallRunning) {
				ResetWallRunning();
			}
		}

		if(OutHit.bBlockingHit) {
			// TODO: 调试.
			NormalOfSurface = OutHit.Normal;

			FVector CharacterUpVector = WRCharacter->GetActorUpVector();
			FVector NewCharacterUpVector = CharacterUpVector;
			FVector ImpactPoint = OutHit.ImpactPoint;
			FVector ActorRightVector = WRCharacter->GetActorRightVector();

			// Check if hit wall is perpendiculer to the player.
			float DotProduct = FVector::DotProduct(CharacterUpVector, NormalOfSurface);

			float WallOffset = 80.f;

			float Min = -0.2f;
			float Max = 0.2f;

			const bool bIsNormal = UKismetMathLibrary::InRange_FloatFloat(DotProduct, Min, Max);

			if(!bIsWallRightHit) {
				NewCharacterUpVector = CharacterUpVector * -1;
			}

			FVector UpVectorNormalize = FVector::CrossProduct(NewCharacterUpVector, NormalOfSurface).GetSafeNormal(0.0001f);

			if(bIsNormal) {
				float RightVectorAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(UpVectorNormalize, ActorRightVector));

				bIsWallRightHit = RightVectorAngle >= 90.0f ? true : false;
				bIsWallLeftHit = RightVectorAngle < 90.0f ? true : false;

				FVector ActorForwardVector = WRCharacter->GetActorForwardVector();

				float ForwardVal = UKismetMathLibrary::DegAcos(FVector::DotProduct(UpVectorNormalize, ActorForwardVector));

				float ForMin = 10.f;
				float ForMax = 40.f;

				const bool bIsForward = UKismetMathLibrary::InRange_FloatFloat(ForwardVal, ForMin, ForMax);

				if(bIsWallRightHit || bIsWallLeftHit) {
					if(bIsForward) {
						bIsCorrentAngle = true;
					}
				}

				if(bIsCorrentAngle) {
					// Sequence 1:
					// Custom Event:
					EnableWallRun();

					FVector ResultVector = NormalOfSurface * WallOffset + ImpactPoint + UpVectorNormalize * Length;

					// Sequence 2:
					FVector InVec = FMath::VInterpConstantTo(WRCharacter->GetActorLocation(), ResultVector, GetWorld()->GetDeltaSeconds(), WRCharacter->GetCharacterMovement()->GetMaxSpeed());

					WRCharacter->SetActorLocation(FVector(InVec.X, InVec.Y, WRCharacter->GetActorLocation().Z));
					WRCharacter->SetActorRotation(UKismetMathLibrary::MakeRotFromX(UpVectorNormalize));

					float lenMin = 0.f;
					float lenMax = 800.f;

					Length = FMath::Clamp(++Length, lenMin, lenMax);

					//// Sequence 3:
					//// Calculate arc path based on sin value of the degree in range between 0-180 then calculate max hieight off set and add it to the player Z axis
					float Interp = bIsInAir ? 10.f : 60.f;

					ZValue = FMath::FInterpConstantTo(ZValue, 180.f, GetWorld()->GetDeltaSeconds(), Interp);

					if(ZValue >= 179.f) {

						bIsWallRunningDone = true;
					} else {
						//float CurrentZ = UKismetMathLibrary::DegAsin(ZValue) * (MaxHeight * GetWorld()->GetDeltaSeconds());

						//if(ZValue < 90.f) {
						//	CurrentZ = ((1.f - UKismetMathLibrary::DegAsin(ZValue)) * -1) * (MaxHeight * GetWorld()->GetDeltaSeconds());
						//}

						//GetCapsuleComponent()->AddWorldOffset(FVector(0.f, 0.f, CurrentZ));
					}
					
				}
			}
		}
	}
}

void AWRCharacter::HitResultImp() {
	
}

void AWRCharacter::ResetWallRunning() {
	WRCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	SpringArmComp->TargetArmLength = 300.0f;

	bIsInAir = false;
	bIsWallLeftHit = false;
	bIsWallRightHit = false;
	bIsCorrentAngle = false;
	bIsWallRunning = false;
	bIsWallRunningDone = false;
	Length = 0.f;
	ZValue = 0.f;
	bIsIgnoreInput = false;
	bIsDoOnce = true;

	OutHit.Reset();
}

void AWRCharacter::EnableWallRun() {
	if(bIsDoOnce) {
		bIsDoOnce = false;
		bIsWallRunning = true;
		bIsIgnoreInput = true;

		SpringArmComp->TargetArmLength = 700.0f;
		WRCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		if(bIsInAir) {
			ZValue = 90.f;
			MaxHeight = 2.f;
		} else {
			ZValue = 0.f;
			MaxHeight = 250.f;
		}
	}
}

// Called to bind functionality to input
void AWRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWRCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AWRCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AWRCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AWRCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &AWRCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWRCharacter::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AWRCharacter::endJump);

	PlayerInputComponent->BindAction("WallRunning", IE_Pressed, this, &AWRCharacter::EnableWallRunning);
	PlayerInputComponent->BindAction("WallRunning", IE_Released, this, &AWRCharacter::DisableWallRunning);
}

