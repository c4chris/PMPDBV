// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabableDynamicMeshActor.h"
#include "DynamicMesh/MeshNormals.h"
#include "DynamicMesh/DynamicAttribute.h"
#include <DynamicMesh/MeshTransforms.h>
#include "MaterialDomain.h"

// Mesh Generators
//include "Generators/MinimalBoxMeshGenerator.h"
#include "Generators/SphereGenerator.h"
//include "Generators/GridBoxMeshGenerator.h"

// Mesh Editing
#include "DynamicMeshEditor.h"

#include "PMPDBV.h"

#include "Log.h"

#include <array>

class AGrabableDynamicMeshActor* AGrabableDynamicMeshActor::curGDMA;

// Sets default values
AGrabableDynamicMeshActor::AGrabableDynamicMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (curGDMA == nullptr)
		curGDMA = this;

	UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor constructor called"));
}

// Called when the game starts or when spawned
void AGrabableDynamicMeshActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor BeginPlay called"));
}

void AGrabableDynamicMeshActor::PostLoad() {
	Super::PostLoad();
	UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor PostLoad called"));
	UE::Geometry::FDynamicMesh3 mesh = GenerateMesh(FVector4f(0, 1, 0, 1));
	UE::Geometry::FDynamicMeshEditor MeshEditor(&mesh);
	const auto mesh2 = GenerateMesh(FVector4f(1, 0, 0, 1));
	FVector3d Point2 = FVector3d(50, 50, 50);
	const auto mesh3 = GenerateMesh(FVector4f(0, 0, 1, 1));
	FVector3d Point3 = FVector3d(-50, 50, 50);
	// append the new sphere via the Editor
	UE::Geometry::FMeshIndexMappings TmpMappings;
	MeshEditor.AppendMesh(&mesh2,TmpMappings,
		[Point2](int32 vid, const FVector3d& Vertex) { return Vertex + Point2; },
		nullptr);
	MeshEditor.AppendMesh(&mesh3,TmpMappings,
		[Point3](int32 vid, const FVector3d& Vertex) { return Vertex + Point3; },
		nullptr);
	const auto validityResult = mesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::Ensure);
	UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor AddAtom Mesh validity is %s, there are %d vertices"), validityResult ? TEXT("true") : TEXT("false"), mesh.VertexCount());
	UpdateMesh(mesh);
	SetupCallbacks();
	GL_init_Atom_Colors();
	FModuleManager::Get().LoadModuleChecked<FPMPDBV>(TEXT("PMPDBV")).OnLoadMsg.AddLambda([&]() {
		Loaded();
		});
}

// Called every frame
void AGrabableDynamicMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UE::Geometry::FDynamicMesh3 AGrabableDynamicMeshActor::GenerateMesh(const FVector4f &color) {
	UE::Geometry::FSphereGenerator SphereGen;
	SphereGen.Radius = 20;
	SphereGen.NumPhi = SphereGen.NumTheta = 10;
	SphereGen.bPolygroupPerQuad = true;
	SphereGen.Generate();
	FDynamicMesh3 SphereMesh(&SphereGen);
	UE::Geometry::FMeshNormals::QuickComputeVertexNormals(SphereMesh);
	SphereMesh.Attributes()->EnablePrimaryColors();
	const auto ColorOverlay = SphereMesh.Attributes()->PrimaryColors();
	for (int VertID : SphereMesh.VertexIndicesItr())
	{
		SphereMesh.SetVertexColor(VertID, color);
		ColorOverlay->AppendElement(color);
	}
	for (int TriID : SphereMesh.TriangleIndicesItr())
	{
		auto Tri = SphereMesh.GetTriangle(TriID);
		ColorOverlay->SetTriangle(TriID, UE::Geometry::FIndex3i(Tri.A, Tri.B, Tri.C));
	}
	const auto vRS = SphereMesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::Ensure);
	UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor RegenerateMesh validity is %s"), vRS ? TEXT("true") : TEXT("false"));
	return SphereMesh;
}

void AGrabableDynamicMeshActor::UpdateMesh(UE::Geometry::FDynamicMesh3& mesh) {
	mesh.EnableAttributes();
	UE::Geometry::FMeshNormals::InitializeOverlayToPerVertexNormals(mesh.Attributes()->PrimaryNormals(), false);
	SourceMesh = mesh;
	auto dmc = GetDynamicMeshComponent();

	if (dmc) {
		*(dmc->GetMesh()) = SourceMesh;
		dmc->NotifyMeshUpdated();
		// update material on new section
		UMaterialInterface* UseMaterial = (dmc->GetMaterial(0) != nullptr) ? dmc->GetMaterial(0) : UMaterial::GetDefaultMaterial(MD_Surface);
		dmc->SetMaterial(0, UseMaterial);
		UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor UpdateMesh succeeded"));
	} else {
		UE_LOG(LogPMPDBV, Warning, TEXT("AGrabableDynamicMeshActor UpdateMesh No Dynamic Mesh Component"));
	}
}

void AGrabableDynamicMeshActor::Loaded()
{
	UE_LOG(LogPMPDBV, Log, TEXT("Received Loaded event"));
	GLRender();
}

void AGrabableDynamicMeshActor::SetupCallbacks()
{
	SwiftCallbacks cb = {
		.GL_atm = GL_atm,
		.GL_bnd = GL_bnd,
		.GL_Atom_Color = GL_Atom_Color,
		.GL_All_Invisible = GL_All_Invisible,
		.GL_ForceRebuild = GL_ForceRebuild,
		.GL_New_Shape = GL_New_Shape,
		.GL_Add_Vertex = GL_Add_Vertex,
		.GL_Draw_Shape = GL_Draw_Shape
	};
	SwiftCBSetup(&cb);
}

void AGrabableDynamicMeshActor::GL_atm(FPoint3D A, short layer, unsigned short group, unsigned short atom, char chain, long color)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_atm(A(%.3f, %.3f, %.3f), layer %hd, group %hd, atom %hd, chain %c, color %08x)"), A.x, A.y, A.z, layer, group, atom, TCHAR(chain), color);
	if (curGDMA != nullptr)
		curGDMA->AddAtom(A, layer, group, atom, chain, color);
}

void AGrabableDynamicMeshActor::GL_bnd(FPoint3D A, FPoint3D B, short layer, unsigned short group, unsigned short atomA, unsigned short atomB, char chain, long color)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_bnd(A(%.3f, %.3f, %.3f), B(%.3f, %.3f, %.3f), layer %hd, group %hd, atomA %hd, atomB %hd, chain %c, color %08x)"), A.x, A.y, A.z, B.x, B.y, B.z, layer, group, atomA, atomB, TCHAR(chain), color);
}

void AGrabableDynamicMeshActor::GL_Atom_Color(char* _Nonnull name, double red, double green, double blue, Boolean isMetallic)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_Atom_Color(name %hs, red %.3f, green %.3f, blue %.3f, %s)"), name, red, green, blue, isMetallic ? TEXT("metallic") : TEXT("not metallic"));
}

void AGrabableDynamicMeshActor::GL_All_Invisible(short layer)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_All_Invisible(layer %hd)"), layer);
}

void AGrabableDynamicMeshActor::GL_ForceRebuild(short layer)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_ForceRebuild(layer %hd)"), layer);
}

void AGrabableDynamicMeshActor::GL_New_Shape(void)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_New_Shape()"));
}

void AGrabableDynamicMeshActor::GL_Add_Vertex(Point3D V, Point3D N, unsigned int color)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_Add_Vertex(V(%.3f, %.3f, %.3f), N(%.3f, %.3f, %.3f), color %08x)"), V.x, V.y, V.z, N.x, N.y, N.z, color);
}

void AGrabableDynamicMeshActor::GL_Draw_Shape(short layer, char type)
{
	UE_LOG(LogPMPDBV, Log, TEXT("GL_Draw_Shape(layer %hd, type %c)"), layer, TCHAR(type));
}

void AGrabableDynamicMeshActor::AddAtom(FPoint3D A, short layer, unsigned short group, unsigned short atom, char chain, long color)
{
	AsyncTask(ENamedThreads::GameThread, [=, this]()
	{
		// Code placed here will run in the game thread 
		UDynamicMeshComponent *dmc = GetDynamicMeshComponent();

		if (dmc) {
			UE::Geometry::FDynamicMesh3 AccumMesh(SourceMesh);
			UE::Geometry::FDynamicMeshEditor MeshEditor(&AccumMesh);
			const auto mesh = GenerateMesh(FVector4f(0, 1, 0, 1));
			//// position/orientation to use to append the box
			FVector3d Point = FVector3d(A.x * 50, A.y * 50, A.z * 50);

			// append the new sphere via the Editor
			UE::Geometry::FMeshIndexMappings TmpMappings;
			MeshEditor.AppendMesh(&mesh,TmpMappings,
				[Point](int32 vid, const FVector3d& Vertex) { return Vertex + Point; },
				nullptr);

			const auto validityResult = AccumMesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::Ensure);
			UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor AddAtom Mesh validity is %s, there are %d vertices"), validityResult ? TEXT("true") : TEXT("false"), AccumMesh.VertexCount());
			//for (int VertID : AccumMesh.VertexIndicesItr())
			//{
			//	FVector3d Position = AccumMesh.GetVertex(VertID);
			//	UE_LOG(LogPMPDBV, Log, TEXT("AccumMesh %d x:%.3f y:%.3f z:%.3f"), VertID, Position.X, Position.Y, Position.Z);
			//}

			UpdateMesh(AccumMesh);
			UE_LOG(LogPMPDBV, Log, TEXT("AGrabableDynamicMeshActor AddAtom %d %d succeeded"), group, atom);
		} else {
			UE_LOG(LogPMPDBV, Warning, TEXT("AddAtom - No Dynamic Mesh Component"));
		}
	});
}
