// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	WeaponType(EWeaponType::EWT_SubMechineGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadMontageSection(FName(TEXT("Reload SMG"))),
	ClipBoneName(TEXT("smg_clip")),
	SlideDisplacement(0.f),
	SlideDisplacementTime(0.2f),
	bMovingSlide(false),
	MaxSlideDisplacement(4.f),
	MaxRecoilRotation(20.f),
	bAutomatic(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	//Update slide on pistol
	UpdateSlideDisplacement();
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f,GetItemMesh()->GetComponentRotation().Yaw,0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	//Direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	//float RandomRotation{ 30.f };
	float RandomRotation = FMath::FRandRange(0.f, 30.f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));

	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

	EnableGlowMaterial();
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath(TEXT("DataTable'/Game/_Game/DataTable/WeaponDataTable.WeaponDataTable'"));
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));
	
	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDateRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_SubMechineGun:
			WeaponDateRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("SubMechineGun")), TEXT(""));
			break;
		case EWeaponType::EWT_AssaultRifle:
			WeaponDateRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("AssaultRifle")), TEXT(""));
			break;
		case EWeaponType::EWT_Pistol:
			WeaponDateRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("Pistol")), TEXT(""));
			break;
		}
		if (WeaponDateRow)
		{
			AmmoType = WeaponDateRow->AmmoType;
			Ammo = WeaponDateRow->WeaponAmmo;
			MagazineCapacity = WeaponDateRow->MagazingCapacity;
			SetPickupSound(WeaponDateRow->PickupSound);
			SetEquipSound(WeaponDateRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDateRow->ItemMesh);
			SetItemName(WeaponDateRow->ItemName);
			SetIconItem(WeaponDateRow->InventoryIcon);
			SetAmmoIcon(WeaponDateRow->AmmoIcon);

			SetMaterialInstance(WeaponDateRow->MaterialInstance);
			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDateRow->MaterialIndex);
			SetClipBoneName(WeaponDateRow->ClipBoneName);
			SetReloadMontageSection(WeaponDateRow->ReloadMontageSection);
			GetItemMesh()->SetAnimClass(WeaponDateRow->AnimBp);
			CrossHairsMiddle = WeaponDateRow->CrossHairsMiddle;
			CrossHairsLeft = WeaponDateRow->CrossHairsLeft;
			CrossHairsRight = WeaponDateRow->CrossHairsRight;
			CrossHairsTop = WeaponDateRow->CrossHairsTop;
			CrossHairsButtom = WeaponDateRow->CrossHairsButtom;
			AutoFireRate = WeaponDateRow->AutoFireRate;
			MuzzleFlash = WeaponDateRow->MuzzleFlash;
			FireSound = WeaponDateRow->FireSound;
			BoneToHide = WeaponDateRow->BoneToHide;
			bAutomatic = WeaponDateRow->bAutomatic;
		}

		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());

			EnableGlowMaterial();
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::UpdateSlideDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		const float ElapsedTimer{ GetWorldTimerManager().GetTimerElapsed(SliderTimer) };
		const float CurveValue{ SlideDisplacementCurve->GetFloatValue(ElapsedTimer) };
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRotation = CurveValue * MaxRecoilRotation;
	}
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::StartSlideTimer()
{
	bMovingSlide = true;
	GetWorldTimerManager().SetTimer(
		SliderTimer,
		this,
		&AWeapon::FinishMovingSlide,
		SlideDisplacementTime
	);
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo+Amount<=MagazineCapacity,TEXT("Attempted to reload with more than magazine capacity!"))
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	
	return Ammo>=MagazineCapacity;
}
