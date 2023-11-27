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

	/** Ĭ�������Ұֵ*/
	float CameraDefaultFOV;

	/** ��׼�ǵ���Ұֵ*/
	float CameraZoomedFOV;

	/** ��ǰ֡�������Ұֵ*/
	float CameraCurrentFOV;

	/** �л���ͷ�Ĳ�ֵ�ٶ�*/
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

public:
	/**FORCEINLINE ��������������ִ������Ծ���٣���ʡ���ܣ�; Return CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	//Return FollowCamera Subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** ���ص��¼� ��/�� ���Ҹ����Ƿ���Լ��*/
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);
};
