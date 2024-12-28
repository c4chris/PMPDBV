// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrameTypes.h"
#include "MathUtil.h"
#include "VectorTypes.h"
#include "DynamicMeshActor.h"
#include "DynamicMeshEditor.h"
#include "MeshAdapterTransforms.h"
extern "C"
{
#include "PMPDBV-Bridging-Header.h"
}
#include "GrabableDynamicMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class PMPDBV_API AGrabableDynamicMeshActor : public ADynamicMeshActor
{
	GENERATED_BODY()

protected:
	/** The SourceMesh used to initialize the mesh Components in the various subclasses */
	FDynamicMesh3 SourceMesh;

public:	
	// Sets default values for this actor's properties
	AGrabableDynamicMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PostLoad() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	static class AGrabableDynamicMeshActor* curGDMA;
	UE::Geometry::FDynamicMesh3 GenerateMesh(const FVector4f& color);
	void UpdateMesh(UE::Geometry::FDynamicMesh3& mesh);
	void Loaded();
	void SetupCallbacks();
	static void GL_atm(FPoint3D A, short layer, unsigned short group, unsigned short atom, char chain, long color);
	static void GL_bnd(FPoint3D A, FPoint3D B, short layer, unsigned short group, unsigned short atomA, unsigned short atomB, char chain, long color);
	static void GL_Atom_Color(char* _Nonnull name, double red, double green, double blue, Boolean isMetallic);
	static void GL_All_Invisible(short layer);
	static void GL_ForceRebuild(short layer);
	static void GL_New_Shape(void);
	static void GL_Add_Vertex(Point3D V, Point3D N, unsigned int color);
	static void GL_Draw_Shape(short layer, char type);
	void AddAtom(FPoint3D A, short layer, unsigned short group, unsigned short atom, char chain, long color);
};
