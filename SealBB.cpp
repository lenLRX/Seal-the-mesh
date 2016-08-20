//////////////////////////////////////////////////////////////////////////////////////////////////////////
//		            SealBB
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CKAll.h"
#include "MyUtility.h"
#include <vector>
#include "Plane.h"
#include "Triangle.h"
#include "Polygon.h"
#include "Seal.h"

CKObjectDeclaration	*FillBehaviorSealBBDecl();
CKERROR CreateSealBBProto(CKBehaviorPrototype **);
int SealBB(const CKBehaviorContext& BehContext);
int SealBBCallBack(const CKBehaviorContext& BehContext);

CKObjectDeclaration	*FillBehaviorSealBBDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("SealBB");	
	
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetVersion(0x00000001);
	od->SetCreationFunction(CreateSealBBProto);
	od->SetDescription("Seal the two side of mesh");
	od->SetCategory("DLVR");
	od->SetGuid(CKGUID(0xec313d2b, 0xa1e82d4c));
	od->SetAuthorGuid(CKGUID(0x56495254,0x4f4f4c53));
	od->SetAuthorName("Virtools");
	od->SetCompatibleClassId(CKCID_BEOBJECT);

	return od;
}

CKERROR CreateSealBBProto(CKBehaviorPrototype** pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("SealBB");
	if (!proto) {
		return CKERR_OUTOFMEMORY;
	}

//---     Inputs declaration
	proto->DeclareInput("In0");

//---     Outputs declaration



//----	Local Parameters Declaration
	proto->DeclareInParameter("Mesh", CKPGUID_MESH );

//----	Settings Declaration
	proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE|CKBEHAVIOR_VARIABLEINPUTS));
	proto->SetBehaviorCallbackFct(SealBBCallBack, CKCB_BEHAVIORATTACH|CKCB_BEHAVIORDETACH|CKCB_BEHAVIORDELETE|CKCB_BEHAVIOREDITED|CKCB_BEHAVIORSETTINGSEDITED|CKCB_BEHAVIORLOAD|CKCB_BEHAVIORPRESAVE|CKCB_BEHAVIORPOSTSAVE|CKCB_BEHAVIORRESUME|CKCB_BEHAVIORPAUSE|CKCB_BEHAVIORRESET|CKCB_BEHAVIORRESET|CKCB_BEHAVIORDEACTIVATESCRIPT|CKCB_BEHAVIORACTIVATESCRIPT|CKCB_BEHAVIORREADSTATE, NULL);
	proto->SetFunction(SealBB);

	*pproto = proto;
	return CK_OK;
}

int SealBB(const CKBehaviorContext& BehContext)
{
	CKBehavior* beh = BehContext.Behavior;
	CKContext* context = beh->GetCKContext();
	CKMesh* mesh = (CKMesh*)beh->GetInputParameterObject(0);

	CKDWORD MeshvStride=0;
	BYTE* MeshVertices = (BYTE*)mesh->GetModifierVertices(&MeshvStride);
	int MeshVCount = mesh->GetModifierVertexCount();

	BYTE* MeshVerticesIterator = MeshVertices;

	std::vector<VxVector> FirstSurface;
	std::vector<int> FirstIdx;
	std::vector<VxVector> SecondSurface;
	std::vector<int> SecondIdx;

	for(int i=0; i<MeshVCount ;i++,MeshVerticesIterator += MeshvStride) {
		//ShowVxVector(context,*(VxVector*)MeshVerticesIterator);
		if(i&1){
			SecondSurface.push_back(*(VxVector*)MeshVerticesIterator);
			SecondIdx.push_back(i);
		}
		else{
			FirstSurface.push_back(*(VxVector*)MeshVerticesIterator);
			FirstIdx.push_back(i);
		}
	}

	std::reverse(SecondSurface.begin(),SecondSurface.end());

	//context->OutputToConsoleEx("first surface");

	for(size_t i = 0; i < FirstSurface.size();i++){
		ShowVxVector(context,FirstSurface[i]);
	}

	//context->OutputToConsoleEx("second surface");

	for(size_t i = 0; i < SecondSurface.size();i++){
		ShowVxVector(context,SecondSurface[i]);
	}
	
	//TODO: put it in parameter
	VxVector FirstNorm(0,1,0);
	VxVector SecondNorm(0,-1,0);

	//TODO:check the number of vertices

	std::vector<int> FirstFace = GenerateFaces(FirstSurface,FirstIdx,FirstNorm,context);
	std::vector<int> SecondFace  = GenerateFaces(SecondSurface,SecondIdx,SecondNorm,context);

	//context->OutputToConsoleEx("FirstFaceCount %d SecondFaceCount %d",FirstFace.size(),SecondFace.size());

	int MeshFaceCount = mesh->GetFaceCount();
	int NextFaceCount = MeshFaceCount + FirstFace.size()/3 + SecondFace.size()/3;

	mesh->SetFaceCount(NextFaceCount);

	CKVINDEX* faceIndex = mesh->GetFacesIndices();

	CKMaterial* mat = mesh->GetMaterial(0);

	int pos = MeshFaceCount;
	for(size_t i = 0;i < FirstFace.size()/3;i++,pos){
		mesh->SetFaceMaterial((i + pos),mesh->GetMaterial(0));
		faceIndex[(i + pos) * 3 + 0] = FirstFace[i * 3 + 0];
		faceIndex[(i + pos) * 3 + 1] = FirstFace[i * 3 + 1];
		faceIndex[(i + pos) * 3 + 2] = FirstFace[i * 3 + 2];
	}

	pos += FirstFace.size()/3;

	

	for(size_t i = 0;i < SecondFace.size()/3;i++,pos){
		mesh->SetFaceMaterial((i + pos),mat);
		faceIndex[(i + pos) * 3 + 0] = SecondFace[i * 3 + 0];
		faceIndex[(i + pos) * 3 + 1] = SecondFace[i * 3 + 1];
		faceIndex[(i + pos) * 3 + 2] = SecondFace[i * 3 + 2];
	}

	mesh->BuildNormals();
	mesh->BuildFaceNormals();
	mesh->NormalChanged();

	//context->OutputToConsoleEx("Done");

	return CKBR_OK;
}

int SealBBCallBack(const CKBehaviorContext& BehContext)
{
	switch (BehContext.CallbackMessage)
	{
		case CKM_BEHAVIORATTACH:
			break;
		case CKM_BEHAVIORDETACH:
			break;
		case CKM_BEHAVIORDELETE:
			break;
		case CKM_BEHAVIOREDITED:
			break;
		case CKM_BEHAVIORSETTINGSEDITED:
			break;
		case CKM_BEHAVIORLOAD:
			break;
		case CKM_BEHAVIORPRESAVE:
			break;
		case CKM_BEHAVIORPOSTSAVE:
			break;
		case CKM_BEHAVIORRESUME:
			break;
		case CKM_BEHAVIORPAUSE:
			break;
		case CKM_BEHAVIORRESET:
			break;
		case CKM_BEHAVIORNEWSCENE:
			break;
		case CKM_BEHAVIORDEACTIVATESCRIPT:
			break;
		case CKM_BEHAVIORACTIVATESCRIPT:
			break;
		case CKM_BEHAVIORREADSTATE:
			break;

	}
	return CKBR_OK;
}


