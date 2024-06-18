// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGeneration/World/VoxelConstructChunkWorld.h"

#include <stdexcept>

#include "../../../Public/WorldGeneration/Chunk/VoxelConstructChunkBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVoxelConstructChunkWorld::AVoxelConstructChunkWorld()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVoxelConstructChunkWorld::BeginPlay()
{
	Super::BeginPlay();

	switch (GenerationType)
	{
	case EWorldGenerationType::GT_3D:
		Generate3DWorld();
		break;
	case EWorldGenerationType::GT_2D:
		Generate2DWorld();
		break;
	default:
		throw std::invalid_argument("Invalid Generation Type");
	}

	UE_LOG(LogTemp, Warning, TEXT("%d Chunks Created"), ChunkCount);
}

void AVoxelConstructChunkWorld::Generate3DWorld()
{
	// Initial chunk generation
	UpdateVisibleChunks(FVector::ZeroVector);
}

void AVoxelConstructChunkWorld::Generate2DWorld()
{
	for (int x = -DrawDistance; x <= DrawDistance; x++)
	{
		for (int y = -DrawDistance; y <= DrawDistance; ++y)
		{
			auto transform = FTransform(
				FRotator::ZeroRotator,
				FVector(x * Size * BlockScale, y * Size * BlockScale, 0),
				FVector::OneVector
			);

			const auto chunk = GetWorld()->SpawnActorDeferred<AVoxelConstructChunkBase>(
				ChunkType,
				transform,
				this
			);

			chunk->GenerationType = EWorldGenerationType::GT_2D;
			chunk->Frequency = Frequency;
			chunk->Material = Material;
			chunk->Size = Size;
			chunk->BlockScale = BlockScale;

			UGameplayStatics::FinishSpawningActor(chunk, transform);

			ChunkCount++;
		}
	}
}


void AVoxelConstructChunkWorld::UpdateVisibleChunks(FVector PlayerPosition)
{
	// Calculate the player's chunk coordinates
	int PlayerChunkX = FMath::RoundToInt(PlayerPosition.X / (Size * BlockScale));
	int PlayerChunkY = FMath::RoundToInt(PlayerPosition.Y / (Size * BlockScale));
	int PlayerChunkZ = FMath::RoundToInt(PlayerPosition.Z / (Size * BlockScale));

	TSet<FIntVector> NewVisibleChunks;

	for (int x = -DrawDistance; x <= DrawDistance; x++)
	{
		for (int y = -DrawDistance; y <= DrawDistance; ++y)
		{
			for (int z = -DrawDistance/2; z <= DrawDistance/2; ++z)
			{
				FIntVector ChunkCoords = FIntVector(PlayerChunkX + x, PlayerChunkY + y, PlayerChunkZ + z);
				NewVisibleChunks.Add(ChunkCoords);

				if (!ActiveChunks.Contains(ChunkCoords))
				{
					// Spawn new chunk
					auto transform = FTransform(
						FRotator::ZeroRotator,
						FVector(ChunkCoords.X * Size * BlockScale, ChunkCoords.Y * Size * BlockScale, ChunkCoords.Z * Size * BlockScale),
						FVector::OneVector
					);

					const auto chunk = GetWorld()->SpawnActorDeferred<AVoxelConstructChunkBase>(
						ChunkType,
						transform,
						this
					);

					chunk->GenerationType = EWorldGenerationType::GT_3D;
					chunk->Frequency = Frequency;
					chunk->Material = Material;
					chunk->Size = Size;
					chunk->BlockScale = BlockScale;

					UGameplayStatics::FinishSpawningActor(chunk, transform);

					ActiveChunks.Add(ChunkCoords, chunk);
					ChunkCount++;
				}
			}
		}
	}

	// Cull chunks that are no longer in the visible area
	for (auto It = ActiveChunks.CreateIterator(); It; ++It)
	{
		if (!NewVisibleChunks.Contains(It.Key()))
		{
			It.Value()->Destroy();
			ActiveChunks.Remove(It.Key());
			ChunkCount--;
		}
	}
}

void AVoxelConstructChunkWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update the visible chunks based on the player's position
	FVector PlayerPosition = GetPlayerPosition();
	//DebugLog
	if (false) {

		//UE_LOG(LogTemp, Warning, TEXT("[%s] Block Modified: %s"), *this->GetActorNameOrLabel(), *Position.ToString());

		if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Red, FString::Printf(TEXT("[%s] Block Modified: %s"), *this->GetActorNameOrLabel(), *PlayerPosition.ToString())); }
	}

	//UpdateVisibleChunks(PlayerPosition);


}

FVector AVoxelConstructChunkWorld::GetPlayerPosition()
{
	FVector PawnLocation = FVector::ZeroVector;
	// Get the player controller
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		// Get the controlled pawn
		APawn* ControlledPawn = PlayerController->GetPawn();
		if (ControlledPawn)
		{
			// Get the location of the pawn
			PawnLocation = ControlledPawn->GetActorLocation();
			// Do something with PawnLocation
		}
	}


	return PawnLocation;
}
