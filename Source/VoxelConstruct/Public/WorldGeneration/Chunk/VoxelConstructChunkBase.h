// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../../Utils/VoxelConstructChunkMeshData.h"
#include "../../Utils/VoxelConstructEnums.h"

#include "VoxelConstructChunkBase.generated.h"


class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class VOXELCONSTRUCT_API AVoxelConstructChunkBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVoxelConstructChunkBase();

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly, Category = "Chunk")
	int Size = 64;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Chunk")
	int BlockScale = 100;

	TObjectPtr<UMaterialInterface> Material;
	float Frequency;
	EWorldGenerationType GenerationType;

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	void ModifyVoxel(const FIntVector Position, const EBlockType Block);


	UFUNCTION(BlueprintCallable, Category = "Chunk")
	void ModifyVoxelBySphere(const FIntVector Position, int32 Radius, const EBlockType Block);

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	virtual void GetBlock(const FVector Position, EBlockType& Block) { Block = EBlockType::Null; };

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	virtual bool isBlock(const FVector Position, const float SurfaceHeight) { return false; };

	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	FIntVector Convert1DTo3D(int32 Index1D, int32 XSize, int32 YSize);

	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	int32 Convert3DTo1D(const FIntVector& Index3D, int32 XSize, int32 YSize);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Setup() PURE_VIRTUAL(AVoxelConstructChunkBase::Setup);
	virtual void Generate2DHeightMap(const FVector Position) PURE_VIRTUAL(AVoxelConstructChunkBase::Generate2DHeightMap);
	virtual void Generate3DHeightMap(const FVector Position) PURE_VIRTUAL(AVoxelConstructChunkBase::Generate3DHeightMap);
	virtual void GenerateMesh() PURE_VIRTUAL(AVoxelConstructChunkBase::GenerateMesh);

	virtual void ModifyVoxelData(const FIntVector Position, const EBlockType Block) PURE_VIRTUAL(AVoxelConstructChunkBase::ModifyVoxelData);

	TObjectPtr<UProceduralMeshComponent> Mesh;
	FastNoiseLite* Noise;
	FVoxelConstructChunkMeshData MeshData;
	int VertexCount = 0;

private:
	void ApplyMesh() const;
	void ClearMesh();
	void GenerateHeightMap();
};