// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Ammo.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	//Base rate for turning/looking up
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	//Turn rates for aiming/not aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	//Mouse look sensitivity scale factors
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookUpRate(0.6f),
	//true when aiming the weapon
	bAiming(false),
	//Camera field of view values
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	//CrossHair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimingFactor(0.f),
	CrosshairShootingFactor(0.f),
	//�ӵ������ʱ������
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	//Automatic fire variables
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	//Item trace variables
	bShoulTraceForItems(false),
	//Camera Interp location Variables
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	//Starting ammo amounts
	Starting9mmAmmo(85),
	StartingARAmmo(120),
	//Combat variable
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMoveSpeed(300.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	bAimingButtonPressed(false),
	//Pick up sound timer properties
	bShouldPlayEquipSound(true),
	bShouldPlayPickupSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a camera boom(pulls in towards the character if there is a collision)*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;//the camera follows at this distance behind Arm
	CameraBoom->bUsePawnControlRotation = true;//Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);
	
	//Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//Attach camera to CameraBoom
	FollowCamera->bUsePawnControlRotation = false;//Camera does not rotate relative

	//Don't rotation when the controller rotates. let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;//Character moves in the ...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//...at this rotation
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Crate Hand Scene Component
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	//Create Interpolations Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	//Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	//Create FInterp Location structs for each interp location. Add to array.
	InitializeInterpLocations();
}

void AShooterCharacter::MoveFoward(float value)
{
	if ((Controller != nullptr) && (value != 0.0f))
	{
		//�ҵ�ǰ������
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if ((Controller != nullptr) && (value != 0.0f))
	{
		//�ҵ���������
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//����������Ϣ�����֡��delta
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());//��/�� * ��/֡
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());//��/�� * ��/֡
}

void AShooterCharacter::Turn(float value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		TurnScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(value * TurnScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		//����׼���ӵ������ʱ��
		StartCrosshairBulletFire();
		//Subtract 1 from the weapon's Ammo
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();
	}	
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{

	//���׼��������ײ
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit)
	{
		//��ͣ����λ�ã����Լ���׷��
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		//û�м�鵽����,OutBeamLocation�����߼����յ�λ��
	}

	//ִ�еڶ��θ��٣�����Ǵ�ǹ��ʼ��
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)//������ǹ�ں�beam�յ���
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;

}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

/**��׼*/ 
void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		//���䵽��׼
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		//���䵽����
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRate()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size()
	);

	//�������״̬�µ�׼��
	if (GetCharacterMovement()->IsFalling())
	{
		//����״̬�»����仯׼��
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else
	{
		//��غ���ٱ仯׼��
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	if (bAiming)
	{
		//������Сһ���׼��
		CrosshairAimingFactor = FMath::FInterpTo(
			CrosshairAimingFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else
	{
		//���ٻָ�׼��
		CrosshairAimingFactor = FMath::FInterpTo(
			CrosshairAimingFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//0.05�������Ϊ��
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimingFactor +
		CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{

	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		//Reload Weapon
		ReloadWeapon();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation)
{
	//��ȡ�ӿڴ�С
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	//��ȡʮ����Ļλ��
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//��ȡʮ�ֵ�����λ�úͷ���
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//��׼�ĵ�����λ�÷�������
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShoulTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}
			//��һ֡��⵽��AItem
			if(TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//��⵽��һ֡��HitItem����һ֡�Ĳ�ͬ
					//����HitItem�ǿյ�
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);					
				}
			}
			//�����崢�浽 TraceHitItemLastFrame ��
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		//�����ص���������һ֡Ӧ�ùر���ʾui
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	//���TSubclassOf ����
	if (DefaultWeaponClass)
	{
		//Spawn the weapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		//Get the hand socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			//Attach the weapon to hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		//set EquippedWeapon to the newly spawned weapon
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRulers(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRulers);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
	}
	
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if(EquippedWeapon==nullptr)	return false;
	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	// Play Fire Sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::SendBullet()
{
	//Send bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd
				);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	//Play hit fire Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon == nullptr) return;

	//Do we have ammo of the correct type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return ;
	if (HandSceneComponent == nullptr) return ;

	//Index for the clip bone on the Equipped Weapon
	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	//Store the transform of the clip
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);

	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l"))); 
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMoveSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),TargetCapsuleHalfHeight,DeltaTime,20.f) };
	
	//Negetive value if crouching; Positive value if standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	
	const FVector MeshOffset{ 0.f,0.f,-DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMoveSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	//check to see if AmmoMap contains Ammo's AmmoType
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		//Get amount of ammo in our AmmoMap for Ammo's Type
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		//Set the amount of ammo in the Map for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		//Check to see if the gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp,0 };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1,0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2,0 };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3,0 };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4,0 };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5,0 };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6,0 };
	InterpLocations.Add(InterpLoc6);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{

	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}
	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount >1) return;
	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Handle interpolation for zoom when aiming
	CameraInterpZoom(DeltaTime);
	//Change look sensitivity based on aiming
	SetLookRate();
	//Calculate Cross hair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
	//Check OverlappedItemCount,item trace for items
	TraceForItems();
	//Interpolate the capsule half height based on crouching/standing
	InterpCapsuleHeight(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);//��������������Ч��ִֹͣ��
	
	//�������
	PlayerInputComponent->BindAxis("MoveFoward", this, &AShooterCharacter::MoveFoward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this,
		&AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this,
		&AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this,
		&AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this,
		&AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this,
		&AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this,
		&AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this,
		&AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this,
		&AShooterCharacter::CrouchButtonPressed);

}

void AShooterCharacter::FinishReloading()
{
	//Update the Combat State
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
	if (EquippedWeapon == nullptr) return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	//Update the AmmoMap
	if (AmmoMap.Contains(EquippedWeapon->GetAmmoType()))
	{
		//Amount of ammo the Character is carrying of the EquippedWeapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];
		//Space left in the magazine of EquippedWeapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			//Reload the magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else 
		{
			//fill the magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShoulTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShoulTraceForItems = true;
	}
}

/* No longer Needed,AItem has GetInterpLocation
FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };
	//Desired = CameraWorldLocation + Forward * A + Up * B

	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}
*/

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
		
	}
	
	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}
