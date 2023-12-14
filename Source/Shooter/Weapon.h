// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MagazingCapacity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsMiddle;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsLeft;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsRight;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsButtom;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsTop;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName BoneToHide;
};

/**
 * 
 */
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

protected:
	
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	
	/** Ammo count for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 Ammo;

	/** Maximum ammo that our weapon can hold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 MagazineCapacity;

	/** The type of weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EWeaponType WeaponType;

	/** The type of Ammo for this Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EAmmoType AmmoType;

	/** FName for the reload montage section */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ReloadMontageSection;

	/** True when moving the clip while reload */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	bool bMovingClip;

	/** Name for the clip bone*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ClipBoneName;

	/** DataTable for weapon properties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	/** Texture for the weapon crosshairs */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UTexture2D* CrossHairsMiddle;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UTexture2D* CrossHairsLeft;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UTexture2D* CrossHairsRight;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UTexture2D* CrossHairsButtom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UTexture2D* CrossHairsTop;

	/** The speed at which automatic fire happens */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	float AutoFireRate;

	/** Particle system spawned at the BarrelSorket */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	/** Sound Played when the weapon is fired */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	USoundCue* FireSound;

	/** Name of the bone to hide on the weapon mesh */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	FName BoneToHide;

public:
	/** Add an impulse to the Weapon */
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	/** Called from Character class when firing weapon */
	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }
	FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

	void ReloadAmmo(int32 Amount);

	void SetMovingClip(bool Move) { bMovingClip = Move; }

	bool ClipIsFull();
	
};
