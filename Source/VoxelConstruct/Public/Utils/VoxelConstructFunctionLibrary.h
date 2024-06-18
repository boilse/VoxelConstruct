// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelConstructFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VOXELCONSTRUCT_API UVoxelConstructFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	static FIntVector WorldToBlockPosition(const FVector& Position, const int BlockScale);

	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	static FIntVector WorldToLocalBlockPosition(const FVector& Position, const int Size, const int BlockScale);

	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	static FIntVector WorldToChunkPosition(const FVector& Position, const int Size, const int BlockScale);

	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	static FIntVector Convert1DTo3D(int32 Index1D, int32 XSize, int32 YSize);

	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category = "VoxelConstruct")
	static int32 Convert3DTo1D(const FIntVector& Index3D, int32 XSize, int32 YSize);
	
};