// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGeneration/Chunk/VoxelConstructChunkBase.h"

#include <stdexcept>

#include "../../../Public/Utils/FastNoiseLite.h"
#include "ProceduralMeshComponent.h"


// Sets default values
AVoxelConstructChunkBase::AVoxelConstructChunkBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();

	// Mesh Settings
	Mesh->SetCastShadow(false);

	// Set Mesh as root
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AVoxelConstructChunkBase::BeginPlay()
{
	Super::BeginPlay();



	Setup();

	GenerateHeightMap();

	GenerateMesh();

	UE_LOG(LogTemp, Warning, TEXT("Vertex Count : %d"), VertexCount);

	ApplyMesh();
}

void AVoxelConstructChunkBase::GenerateHeightMap()
{
	switch (GenerationType)
	{
	case EWorldGenerationType::GT_3D:
		Generate3DHeightMap(GetActorLocation() / BlockScale);
		break;
	case EWorldGenerationType::GT_2D:
		Generate2DHeightMap(GetActorLocation() / BlockScale);
		break;
	default:
		throw std::invalid_argument("Invalid Generation Type");
	}
}

void AVoxelConstructChunkBase::ApplyMesh() const
{
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(
		0,
		MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Normals,
		MeshData.UV0,
		MeshData.Colors,
		TArray<FProcMeshTangent>(),
		true
	);
}

void AVoxelConstructChunkBase::ClearMesh()
{
	VertexCount = 0;
	MeshData.Clear();
}

void AVoxelConstructChunkBase::ModifyVoxel(const FIntVector Position, const EBlockType Block)
{


	if (Position.X >= Size || Position.Y >= Size || Position.Z >= Size || Position.X < 0 || Position.Y < 0 || Position.Z < 0)
	{
		return;
	}

	//DebugLog
	if (false) {

		UE_LOG(LogTemp, Warning, TEXT("[%s] Block Modified: %s"), *this->GetActorNameOrLabel(), *Position.ToString());

		if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Red, FString::Printf(TEXT("[%s] Block Modified: %s"), *this->GetActorNameOrLabel(), *Position.ToString())); }
	}

	ModifyVoxelData(Position, Block);

	ClearMesh();

	GenerateMesh();

	ApplyMesh();
}
// FIntVector(GetActorLocation()/BlockScale)
void AVoxelConstructChunkBase::ModifyVoxelBySphere(const FIntVector Position, int32 Radius, const EBlockType Block)
{
	// Calculate the local position within the chunk
	FIntVector LocalPosition = Position- FIntVector(GetActorLocation() / BlockScale);

	// Calculate the bounds for the sphere within the chunk
	int32 MinX = FMath::Max(0, LocalPosition.X - Radius);
	int32 MaxX = FMath::Min(Size - 1, LocalPosition.X + Radius);
	int32 MinY = FMath::Max(0, LocalPosition.Y - Radius);
	int32 MaxY = FMath::Min(Size - 1, LocalPosition.Y + Radius);
	int32 MinZ = FMath::Max(0, LocalPosition.Z - Radius);
	int32 MaxZ = FMath::Min(Size - 1, LocalPosition.Z + Radius);

	// Iterate through all points in the bounding box
	for (int32 X = MinX; X <= MaxX; ++X)
	{
		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			for (int32 Z = MinZ; Z <= MaxZ; ++Z)
			{
				// Calculate the local voxel position
				FIntVector VoxelPosition(X, Y, Z);


				// Check if the voxel is within the sphere
				int32 DistanceSquared = (VoxelPosition.X - LocalPosition.X) * (VoxelPosition.X - LocalPosition.X) +
					(VoxelPosition.Y - LocalPosition.Y) * (VoxelPosition.Y - LocalPosition.Y) +
					(VoxelPosition.Z - LocalPosition.Z) * (VoxelPosition.Z - LocalPosition.Z);

				if (DistanceSquared <= Radius * Radius)
				{
					if (!(VoxelPosition.X >= Size || VoxelPosition.Y >= Size || VoxelPosition.Z >= Size || VoxelPosition.X < 0 || VoxelPosition.Y < 0 || VoxelPosition.Z < 0))
					{
						ModifyVoxelData(VoxelPosition, Block); // Use global position here
					}
				}
			}
		}
	}

	// Clear and regenerate the mesh
	ClearMesh();
	GenerateMesh();
	ApplyMesh();
}


FIntVector AVoxelConstructChunkBase::Convert1DTo3D(int32 Index1D, int32 XSize, int32 YSize)
{
	if (Index1D < 0)
	{

		return FIntVector::ZeroValue;
	}

	if (XSize <= 0 || YSize <= 0)
	{

		return FIntVector::ZeroValue;
	}

	int32 X = Index1D % XSize;
	int32 Y = (Index1D / XSize) % YSize;
	int32 Z = (Index1D / (XSize * YSize));

	return FIntVector{ X, Y, Z };
}

int32 AVoxelConstructChunkBase::Convert3DTo1D(const FIntVector& Index3D, int32 XSize, int32 YSize)
{
	const bool bInvalidBounds =
		Index3D.X < 0 ||
		Index3D.X >= XSize ||
		Index3D.Y < 0 ||
		Index3D.Y >= YSize ||
		Index3D.Z < 0
		;

	if (bInvalidBounds)
	{
		return 0;
	}

	if (XSize <= 0 || YSize <= 0)
	{
		return 0;
	}

	return Index3D.X + (Index3D.Y * XSize) + (Index3D.Z * XSize * YSize);
}