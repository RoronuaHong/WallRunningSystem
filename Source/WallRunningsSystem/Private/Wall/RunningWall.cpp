// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall/RunningWall.h"

// Sets default values
ARunningWall::ARunningWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_GameTraceChannel1);
}

// Called when the game starts or when spawned
void ARunningWall::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARunningWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

