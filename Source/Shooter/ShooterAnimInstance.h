// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"


UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};
/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:

	/** Handle turning in place variables */
	void TurnInPlace();
	
	/**Handle calculations for leaning while running */
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	//the speed of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	//��ɫ�Ƿ��ڿ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	//��ɫ�Ƿ����ƶ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	//����ɨ���ƫ��ƫ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;
	
	//����ɨ���ƫ��ƫ����ͣ�µ�ǰ��֡
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;


	/** Yaw of the Character this frame,Only update when standing still and not in air*/
	float TIPCharacterYaw;

	/** Yaw of the Character the previous frame,Only update when standing still and not in air*/
	float TIPCharacterYawLastframe;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float RootYawOffset;

	/** Rotation curve value this frame*/
	float RotationCurve;

	/** Rotation curve value last frame*/
	float RotationCurveLastFrame;

	/** the Pitch of the aim rotation,used for aim offset*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float Pitch;

	/** true when reloading,used to prevent aim offset while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	bool bReloading;

	/** Offset state; used to determie which Aim offset to use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	EOffsetState OffsetState;

	/** Character Yaw this frame */
	FRotator CharacterRotation;

	/** Character Yaw last frame */
	FRotator CharacterRotationLastframe;

	/** Yaw delta used for leaning in the running BlendSpace */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"));
	float YawDelta;

	/** True when Crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess = "true"));
	bool bCrouching;

	/** True when Equipping */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess = "true"));
	bool bEquipping;

	/** Change the recoil weight based on turning in place and aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess = "true"));
	float RecoilWeight;

	/** Turn when turning in place */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess = "true"));
	bool bTurningInplace;

	/** Weapon Type for the currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess = "true"));
	EWeaponType EquippedWeaponType;
};
