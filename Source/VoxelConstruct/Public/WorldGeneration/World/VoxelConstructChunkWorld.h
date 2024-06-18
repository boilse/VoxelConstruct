// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../../Utils/VoxelConstructEnums.h"
#include "../Chunk/VoxelConstructChunkBase.h"


#include "VoxelConstructChunkWorld.generated.h"



UCLASS()
class VOXELCONSTRUCT_API AVoxelConstructChunkWorld : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "World")
	TSubclassOf<AVoxelConstructChunkBase> ChunkType;

	UPROPERTY(EditAnywhere, Category = "World")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunksPerFrame;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	int Size = 32;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	int BlockScale = 100;

	UPROPERTY(EditAnywhere, Category = "Height Map")
	EWorldGenerationType GenerationType;

	UPROPERTY(EditAnywhere, Category = "Height Map")
	float Frequency = 0.03f;

	UPROPERTY(VisibleAnywhere, Category = "Height Map")
	TMap<FIntVector, AVoxelConstructChunkBase*> ActiveChunks;

	UPROPERTY(VisibleAnywhere, Category = "Height Map")
	TArray<FIntVector> ChunksToSpawn;
	
	UPROPERTY(VisibleAnywhere, Category = "Height Map")
	int32 ChunkCount;




	// Sets default values for this actor's properties
	AVoxelConstructChunkWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	void SpawnChunk(FIntVector ChunkCoords);

	void Generate3DWorld();
	void Generate2DWorld();
	void UpdateVisibleChunks(FVector PlayerPosition);
	FVector GetPlayerPosition();
};