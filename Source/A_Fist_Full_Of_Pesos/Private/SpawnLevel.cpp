// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnLevel.h"

#include "BaseLevel.h"
#include "Engine.h"
#include "Components/BoxComponent.h"
#include "RunnerCharacter.h"

// Sets default values
ASpawnLevel::ASpawnLevel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnLevel::BeginPlay()
{
	Super::BeginPlay();

	SpawnLevel(true);
	SpawnLevel(false);
	SpawnLevel(false);
	SpawnLevel(false);



}

// Called every frame
void ASpawnLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnLevel::SpawnLevel(bool IsFirst)
{

	SpawnOffset = FMath::RandRange(SpawnOffsetMin, SpawnOffsetMax);
	SpawnLocation = FVector(0.0f, SpawnOffset, 0.0f);
	SpawnRotation = FRotator(0, 90, 0);

	if (!IsFirst)
	{
		ABaseLevel* LastLevel = LevelList.Last();
		SpawnLocation += LastLevel->GetSpawnLocation()->GetComponentTransform().GetTranslation();
	}

	RandomLevel = FMath::RandRange(1, 5);
	ABaseLevel* NewLevel = nullptr;

	if (RandomLevel == 1)
	{
		NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level1,
			SpawnLocation, SpawnRotation, SpawnInfo);
	}
	else if (RandomLevel == 2)
	{
		NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level2,
			SpawnLocation, SpawnRotation, SpawnInfo);
	}
	else if (RandomLevel == 3)
	{
		NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level3,
			SpawnLocation, SpawnRotation, SpawnInfo);
	}
	else if (RandomLevel == 4)
	{
		NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level4,
			SpawnLocation, SpawnRotation, SpawnInfo);
	}
	else if (RandomLevel == 5)
	{
		NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level5,
			SpawnLocation, SpawnRotation, SpawnInfo);
	}
	//else if (RandomLevel == 6)
	//{
	//	NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level6,
	//		SpawnLocation, SpawnRotation, SpawnInfo);
	//}
	//else if (RandomLevel == 7)
	//{
	//	NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level7,
	//		SpawnLocation, SpawnRotation, SpawnInfo);
	//}
	//else if (RandomLevel == 8)
	//{
	//	NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level8,
	//		SpawnLocation, SpawnRotation, SpawnInfo);
	//}
	//else if (RandomLevel == 9)
	//{
	//	NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level9,
	//		SpawnLocation, SpawnRotation, SpawnInfo);
	//}
	//else if (RandomLevel == 10)
	//{
	//	NewLevel = GetWorld()->SpawnActor<ABaseLevel>(Level10,
	//		SpawnLocation, SpawnRotation, SpawnInfo);
	//}
   // Si se gener� un nuevo nivel
	if (NewLevel != nullptr)
	{
		if (NewLevel->GetTrigger())
		{
			NewLevel->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &ASpawnLevel::OnOverlapBegin);
		}
	}
	if (NewLevel != nullptr)
	{
		// Agrega el nuevo nivel a la lista de niveles generados
		LevelList.Add(NewLevel);
	}

}

void ASpawnLevel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ARunnerCharacter::StaticClass()))
	{
		SpawnLevel(false);
	}	
}

