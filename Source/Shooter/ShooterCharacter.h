// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "9mm"),
	ECS_FireTimerInProgress UMETA(DisplayName="FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName="Reloading"),
	ECS_NAX UMETA(DisplayName = "DEfaultMAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	//Scene Component to use for its location for interping
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//Number of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//����ǰ������ʱ����
	void MoveFoward(float value);

	//��������ʱ����
	void MoveRight(float value);

	/**
	*����ת�������Ը���������ת��
	* @param Rate ����һ����һ�������ʣ�1.0��ʾ100%
	*/
	void TurnAtRate(float Rate);

	/**
	*����̧ͷ�����Ը���������ת��
	* @param Rate ����һ����һ�������ʣ�1.0��ʾ100%
	*/
	void LookUpAtRate(float Rate);

	/**
	*ͨ�����X����ת������
	* @param value �������Ĳ���ֵ
	*/
	void Turn(float value);

	/**
	*ͨ�����Y����ת������
	* @param value �������Ĳ���ֵ
	*/
	void LookUp(float value);

	//���������ʱ����
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation,FVector& OutBeamLocation);

	/** ͨ����ť����bAmingΪ����*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	//��׼ʱ���û�����ת���ʺͻ���̧ͷ����
	void SetLookRate();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/** Line trace for items under the Crosshairs*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation);

	/** Trace for items if overlappedItemCount > 0*/
	void TraceForItems();

	/** Spawn a default weapon and equips it*/
	class AWeapon* SpawnDefaultWeapon();

	/** Takes a weapon and attaches it to the mesh*/
	void EquipWeapon(AWeapon* WeaponToEquip);

	/** Detach weapon and let it fall to the ground*/
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	/** Drops currently equipped Weapon and Equips TraceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Initialize the Ammo Map with ammo values */
	void InitializeAmmoMap();

	/** Check to make sure out weapon has ammo */
	bool WeaponHasAmmo();

	/** FireWeapon Functions */
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();
	/** Bound to the R key and Gamepad Face Button left*/
	void ReloadButtonPressed();
	/** Handle Reloading of the weapon */
	void ReloadWeapon();

	/** Checks to see if we have ammo of the EquippedWeapon's ammo type*/
	bool CarryingAmmo();

	/** Called from animation blueprint with grab clip notify */
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/** Called from animation blueprint with release clip notify */
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	/** Interps capsule half height when crouching/standing */
	void InterpCapsuleHeight(float DeltaTime);

	void Aim();
	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocations();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:

	/**��class����ȥinclude; ���ɱ�������ڽ�������� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	/**�������������ŵ��ɱ�*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//����ת�����ʣ��� ��/�� Ϊ��λ���������ſ��ܻ�Ӱ�����ս��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	//����̧ͷ���ʣ��� ��/�� Ϊ��λ���������ſ��ܻ�Ӱ�����ս��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** ����״̬�µ���ת����*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/** ����״̬�µ�̧ͷ����*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/** ��׼״̬�µ���ת����*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/** ��׼״̬�µ���ת����*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;
	
	//����׼��ʱ�������������ת�ʵı�������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	//����׼��ʱ�����������̧ͷ�ʵı�������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;
	
	//��׼��ʱ�������������ת�ʵı�������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	//��׼��ʱ�����������̧ͷ�ʵı�������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	
	/** ���ǹ��*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	/** Flash spawned at BarralSocket*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	/** Montage for firing the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	/**Particles spawned upon bullet impact*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactParticles;

	/**Smoke trail for bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* BeamParticles;

	/** True when Aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	/** Default camera field of view valueֵ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CameraDefaultFOV;

	/** Field of view value for when zoomed in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CameraZoomedFOV;

	/** Current field of view this frame */
	float CameraCurrentFOV;

	/** Interp speed for zooming when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomInterpSpeed;

	/** ȷ��ʮ��׼�ߵ���ɢ */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** ʮ��׼��չ�����ٶȷ���*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** ����ʮ��׼��չ�����ٶȷ���*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/** ��׼ʱʮ��׼��չ�����ٶȷ���*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimingFactor;

	/** ����ʮ��׼��չ����������*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/** ������»����ֱ��ұߴ���������*/
	bool bFireButtonPressed;

	/** �ܿ���ʱΪ�棬�ȴ�����ʱΪ��*/
	bool bShouldFire;

	/** �Զ���������*/
	float AutomaticFireRate;

	/** ���ÿ������ʱ��*/
	FTimerHandle AutoFireTimer;

	/** Ӧ�ü��ʱΪ��*/
	bool bShoulTraceForItems;

	/** �ص����������*/
	int8 OverlappedItemCount;

	/** ��һ֡��⵽������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	class AItem* TraceHitItemLastFrame;

	/** Currently equipment Weapon*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/** Set this in Blueprint for the default Weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
	/** The item currently hit by our trace int TraceForItems(could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/** Distance outward form the camera for the Interp destination*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	/** Distance Upward form the camera for the Interp destination*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	/** Map to keep track of ammo of the different ammo types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** Starting amount of 9mm ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/** Starting amount of AR ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/** Combat State, can only fire or reload if Unoccupied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	ECombatState CombatState;

	/** Montage for Reload animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/** Transform of the clip when we first grab the clip during reloading */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FTransform ClipTransform;

	/** Scene component to attach to the character's hand during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	USceneComponent* HandSceneComponent;

	/** true when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bCrouching;

	/** Regular movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float BaseMovementSpeed;

	/** Crouch movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float CrouchMoveSpeed;

	/** Current half height of the capsule */
	float CurrentCapsuleHalfHeight;

	/** Half height of the capsule when not crouching */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Movement, meta = (AllowPrivateAccess = "true"));
	float StandingCapsuleHalfHeight;

	/** Half height of the capsule when crouching */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Movement, meta = (AllowPrivateAccess = "true"));
	float CrouchingCapsuleHalfHeight;

	/** Ground friction while not crouching */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Movement, meta = (AllowPrivateAccess = "true"));
	float BaseGroundFriction;

	/** Ground friction while crouching */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Movement, meta = (AllowPrivateAccess = "true"));
	float CrouchingGroundFriction;

	/** Used for knowing when the aiming button is pressed */
	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp5;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* InterpComp6;

	/** Array of interp location structs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	/** Time to wait before we can play another pickup sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	float PickupSoundResetTime;
	/** Time to wait before we can play another equip sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	float EquipSoundResetTime;

	/** An array for AItems for our Inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"));
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	/** Delegate for sending slot information to InventoryBar when equipping */
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"));
	FEquipItemDelegate EquipItemDelegate;

public:
	/** Return CameraBoom Subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	//Return FollowCamera Subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** ���ص��¼� ��/�� ���Ҹ����Ƿ���Լ��*/
	void IncrementOverlappedItemCount(int8 Amount);

	//no longer needed;AItem has GetInterpLocation
	//FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FInterpLocation GetInterpLocation(int32 Index);

	//Returns the index in InterpLocations array with the lowest ItemCount
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index,int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

};
