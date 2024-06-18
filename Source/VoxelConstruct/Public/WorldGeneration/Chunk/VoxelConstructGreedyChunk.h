// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGeneration/Chunk/VoxelConstructChunkBase.h"

#include "../../Utils/VoxelConstructEnums.h"

#include "VoxelConstructGreedyChunk.generated.h"


class FastNoiseLite;
class UProceduralMeshComponent;

/**
 * 
 */
UCLASS()
class VOXELCONSTRUCT_API AVoxelConstructGreedyChunk : public AVoxelConstructChunkBase
{
	GENERATED_BODY()

	struct FMask
	{
		EBlockType Block;
		int Normal;
	};

protected:
	virtual void Setup() override;
	virtual void Generate2DHeightMap(FVector Position) override;
	virtual void Generate3DHeightMap(FVector Position) override;
	virtual void GenerateMesh() override;
	virtual void ModifyVoxelData(const FIntVector Position, const EBlockType Block) override;
	virtual void GetBlock(const FVector Position, EBlockType& Block) override;

	virtual bool isBlock(const FVector Position, const float SurfaceHeight) override;

private:
	TArray<EBlockType> Blocks;
	
	bool GetTree(FIntVector BlockPos);


	void CreateQuad(FMask Mask, FIntVector AxisMask, int Width, int Height, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4);
	int GetBlockIndex(int X, int Y, int Z) const;
	EBlockType BlockInBounds(FIntVector Index) const;
	bool CompareMask(FMask M1, FMask M2) const;
	int GetTextureIndex(EBlockType Block, FVector Normal) const;
};

