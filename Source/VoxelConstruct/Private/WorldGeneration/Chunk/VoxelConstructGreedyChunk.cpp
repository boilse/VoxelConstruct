// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGeneration/Chunk/VoxelConstructGreedyChunk.h"

#include "../../../Public/Utils/FastNoiseLite.h"


void AVoxelConstructGreedyChunk::Setup()
{
	// Initialize Blocks
	Blocks.SetNum(Size * Size * Size);
}

void AVoxelConstructGreedyChunk::Generate2DHeightMap(const FVector Position)
{
	for (int x = 0; x < Size; x++)
	{
		for (int y = 0; y < Size; y++)
		{
			const float Xpos = x + Position.X;
			const float Ypos = y + Position.Y;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) + 1) * Size / 2), 0, Size);

			for (int z = 0; z < Size; z++)
			{
				if (z < Height - 3) Blocks[GetBlockIndex(x, y, z)] = EBlockType::Stone;
				else if (z < Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlockType::Dirt;
				else if (z == Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlockType::Grass;
				else Blocks[GetBlockIndex(x, y, z)] = EBlockType::Air;
			}
		}
	}
}

void AVoxelConstructGreedyChunk::Generate3DHeightMap(const FVector Position)
{
	for (int32 Idx = 0; Idx < Size * Size * Size; ++Idx)
	{
		FIntVector Idx3D = Convert1DTo3D(Idx, Size, Size);


		GetBlock(FVector(
			Idx3D.X + Position.X,
			Idx3D.Y + Position.Y,
			Idx3D.Z + Position.Z),
			Blocks[Idx]);
	}

	for (int32 Idx = 0; Idx < Size * Size * Size; ++Idx)
	{
		FIntVector Idx3D = Convert1DTo3D(Idx, Size, Size);


		switch (Blocks[Idx])
		{
		case EBlockType::stTree:
		{
			for (int32 x = 0; x < 6; x++)
			{
				for (int32 y = 0; y < 6; y++)
				{
					for (int32 z = 0; z < 10; z++)
					{
						EBlockType TreeBlock = GetTree(FIntVector(x, y, z))? EBlockType::Grass: EBlockType::Air;


						int32 blockid = Convert3DTo1D( Idx3D + FIntVector(x-3,y-3,z), Size, Size);

						if (blockid >= 0 && blockid < Blocks.Num())
							Blocks[blockid] = TreeBlock;
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

void AVoxelConstructGreedyChunk::GetBlock(const FVector Position, EBlockType& Block)
{
	float NoiseValue;

	Noise->SetFrequency(0.002);
	Noise->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	float SurfaceHeight = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Position.X, Position.Y) + 1) * Size / 2), 0, Size);




	Noise->SetFrequency(.999);
	Noise->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	Noise->SetFractalType(FastNoiseLite::FractalType_PingPong);
	NoiseValue = Noise->GetNoise(Position.X, Position.Y);

	bool bTree = (Position.Z == SurfaceHeight + 1) && (!(FMath::Abs(NoiseValue) >= 0.001) && FMath::Abs(NoiseValue) < 0.005);


	if (!bTree)
	{
		if (!isBlock(Position, SurfaceHeight))
		{
			Block = EBlockType::Air;
			return;
		}

		if (Position.Z < 100 - 100) Block = EBlockType::Stone;
		else if (Position.Z < 100 - 10) Block = EBlockType::Dirt;
		else if (Position.Z <= 100 - 1) Block = EBlockType::Grass;
		else Block = EBlockType::Air;

	}
	else { Block = EBlockType::stTree; }

}

bool AVoxelConstructGreedyChunk::isBlock(const FVector Position, const float SurfaceHeight)
{
	float NoiseValue;


	Noise->SetFrequency(0.005);
	Noise->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);
	NoiseValue = Noise->GetNoise(Position.X, Position.Y, Position.Z);

	bool bSurfaceBlock = Position.Z <= SurfaceHeight && Position.Z > SurfaceHeight - 90;
	bool bCavesBlock = (NoiseValue >= 0) && Position.Z <= SurfaceHeight - 50;



	bool bResult = bCavesBlock || bSurfaceBlock;

	return bResult;
}

bool AVoxelConstructGreedyChunk::GetTree(FIntVector BlockPos)
{
	TArray<bool> aTree = {
	0,0,0,0,0,0,
	0,0,1,1,0,0,
	0,1,1,1,1,0,
	0,1,1,1,1,0,
	0,0,1,1,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,

	0,0,1,1,0,0,
	0,1,1,1,1,0,
	1,1,1,1,1,1,
	1,1,1,1,1,1,
	0,1,1,1,1,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,

	0,0,1,1,0,0,
	1,1,1,1,1,0,
	1,1,1,1,1,1,
	1,1,1,1,1,1,
	0,1,1,1,1,0,
	0,0,1,1,0,1,
	0,0,1,1,0,0,
	0,0,1,1,0,0,
	0,0,1,1,0,0,
	0,0,1,1,0,0,

	0,0,1,1,0,0,
	1,1,1,1,1,1,
	1,1,1,1,1,1,
	1,1,1,1,1,0,
	1,1,1,1,1,1,
	0,0,1,1,0,0,
	0,0,1,1,0,0,
	0,0,1,1,0,0,
	0,0,1,1,0,0,
	0,0,1,1,0,0,

	0,0,1,1,0,0,
	0,1,1,1,1,0,
	1,1,1,1,1,1,
	1,1,1,1,1,1,
	0,1,1,1,1,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,

	0,0,0,0,0,0,
	0,0,1,1,0,0,
	0,1,1,1,1,0,
	0,1,1,1,1,0,
	0,0,1,1,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,
	0,0,0,0,0,0,

	};

	int32 blockid = Convert3DTo1D(BlockPos, 6, 10);
	if(aTree.IsValidIndex(blockid))
	return aTree[blockid]; 
	
	return false;// No block present
}

void AVoxelConstructGreedyChunk::GenerateMesh()
{
	// Single allocation of Mask
	TArray<FMask> Mask;
	Mask.SetNum(Size * Size);

	// Sweep over each axis (X, Y, Z)
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		// 2 Perpendicular axis
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		const int MainAxisLimit = Size;
		const int Axis1Limit = Size;
		const int Axis2Limit = Size;

		auto DeltaAxis1 = FIntVector::ZeroValue;
		auto DeltaAxis2 = FIntVector::ZeroValue;

		auto ChunkItr = FIntVector::ZeroValue;
		auto AxisMask = FIntVector::ZeroValue;

		AxisMask[Axis] = 1;

		// Check each slice of the chunk
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
		{
			int N = 0;

			// Compute Mask
			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const auto CurrentBlock = BlockInBounds(ChunkItr);
					const auto ComparEBlockType = BlockInBounds(ChunkItr + AxisMask);

					const bool CurrentBlockOpaque = CurrentBlock != EBlockType::Air;
					const bool ComparEBlockTypeOpaque = ComparEBlockType != EBlockType::Air;

					if (CurrentBlockOpaque == ComparEBlockTypeOpaque)
					{
						Mask[N++] = FMask{ EBlockType::Null, 0 };
					}
					else if (CurrentBlockOpaque)
					{
						Mask[N++] = FMask{ CurrentBlock, 1 };
					}
					else
					{
						Mask[N++] = FMask{ ComparEBlockType, -1 };
					}
				}
			}

			++ChunkItr[Axis];
			N = 0;

			// Generate Mesh From Mask
			for (int j = 0; j < Axis2Limit; ++j)
			{
				for (int i = 0; i < Axis1Limit;)
				{
					if (Mask[N].Normal != 0)
					{
						const auto CurrentMask = Mask[N];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int Width;

						for (Width = 1; i + Width < Axis1Limit && CompareMask(Mask[N + Width], CurrentMask); ++Width)
						{
						}

						int Height;
						bool Done = false;

						for (Height = 1; j + Height < Axis2Limit; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (!CompareMask(Mask[N + k + Height * Axis1Limit], CurrentMask))
								{
									Done = true;
									break;
								}
							}

							if (Done) break;
						}

						DeltaAxis1[Axis1] = Width;
						DeltaAxis2[Axis2] = Height;

						CreateQuad(
							CurrentMask, AxisMask, Width, Height,
							ChunkItr,
							ChunkItr + DeltaAxis1,
							ChunkItr + DeltaAxis2,
							ChunkItr + DeltaAxis1 + DeltaAxis2
						);

						DeltaAxis1 = FIntVector::ZeroValue;
						DeltaAxis2 = FIntVector::ZeroValue;

						for (int l = 0; l < Height; ++l)
						{
							for (int k = 0; k < Width; ++k)
							{
								Mask[N + k + l * Axis1Limit] = FMask{ EBlockType::Null, 0 };
							}
						}

						i += Width;
						N += Width;
					}
					else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}


void AVoxelConstructGreedyChunk::CreateQuad(
	const FMask Mask,
	const FIntVector AxisMask,
	const int Width,
	const int Height,
	const FIntVector V1,
	const FIntVector V2,
	const FIntVector V3,
	const FIntVector V4
)
{
	const auto Normal = FVector(AxisMask * Mask.Normal);
	const auto Color = FColor(0, 0, 0, GetTextureIndex(Mask.Block, Normal));

	MeshData.Vertices.Append({
		FVector(V1) * BlockScale,
		FVector(V2) * BlockScale,
		FVector(V3) * BlockScale,
		FVector(V4) * BlockScale
		});

	MeshData.Triangles.Append({
		VertexCount,
		VertexCount + 2 + Mask.Normal,
		VertexCount + 2 - Mask.Normal,
		VertexCount + 3,
		VertexCount + 1 - Mask.Normal,
		VertexCount + 1 + Mask.Normal
		});

	MeshData.Normals.Append({
		Normal,
		Normal,
		Normal,
		Normal
		});

	MeshData.Colors.Append({
		Color,
		Color,
		Color,
		Color
		});

	if (Normal.X == 1 || Normal.X == -1)
	{
		MeshData.UV0.Append({
			FVector2D(Width, Height),
			FVector2D(0, Height),
			FVector2D(Width, 0),
			FVector2D(0, 0),
			});
	}
	else
	{
		MeshData.UV0.Append({
			FVector2D(Height, Width),
			FVector2D(Height, 0),
			FVector2D(0, Width),
			FVector2D(0, 0),
			});
	}

	VertexCount += 4;
}

void AVoxelConstructGreedyChunk::ModifyVoxelData(const FIntVector Position, const EBlockType Block)
{

	const int Index = GetBlockIndex(Position.X, Position.Y, Position.Z);

	Blocks[Index] = Block;

	//DebugLog
	if (false) {

		UE_LOG(LogTemp, Warning, TEXT("[%s] Block Modified: %d"), *this->GetActorNameOrLabel(), ((int32)Blocks[Index]));

		if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Red, FString::Printf(TEXT("[%s] Block Modified: %s"), *this->GetActorNameOrLabel(), *Position.ToString())); }
	}
}

int AVoxelConstructGreedyChunk::GetBlockIndex(const int X, const int Y, const int Z) const
{
	return Z * Size * Size + Y * Size + X;
}

EBlockType AVoxelConstructGreedyChunk::BlockInBounds(const FIntVector Index) const
{
	if (Index.X >= Size || Index.Y >= Size || Index.Z >= Size || Index.X < 0 || Index.Y < 0 || Index.Z < 0)
		return EBlockType::Air;
	return Blocks[GetBlockIndex(Index.X, Index.Y, Index.Z)];
}

bool AVoxelConstructGreedyChunk::CompareMask(const FMask M1, const FMask M2) const
{
	return M1.Block == M2.Block && M1.Normal == M2.Normal;
}

int AVoxelConstructGreedyChunk::GetTextureIndex(const EBlockType Block, const FVector Normal) const
{
	switch (Block) {
	case EBlockType::Grass:
	{
		if (Normal == FVector::UpVector) return 0;
		return 1;
	}
	case EBlockType::Dirt: return 2;
	case EBlockType::Stone: return 3;
		//case EBlockType::stTree: return 255;
	default: return 255;
	}
}
