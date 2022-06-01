// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// ShooterCharacter유효한 지 체크해서 없으면 유효하게 해준다.
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	// 유효한 지 다시 한번 검사
	if (ShooterCharacter)
	{
		// Get the lateral speed of ther character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size(); // 속도의 크기는 속력

		// is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is the character accelerating?
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		// GetBaseAimRotation()은 컨트롤러가 어떠한 축(여기서는 x축을 기준으로 Yaw회전을 얼마나 했는지)을 기준으로
		// 얼마나 회전했는 지를 반환
		// UKismetMathLibrary::MakeRotFromX() : 입력 파라미터 벡터를 X축으로 하여 새로운 기저를 구축하고, 그 기저의 회전 행렬을 리턴한다.
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// UKismetMathLibrary::NormalizedDeltaRotator()는 AimRotation을 기준으로 MovementRotation가 얼만큼 회전한 상태인 지를 나타냄
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// LastMovementOffsetYaw = ShooterCharacter->GetVelocity().Size() > 0.f ? MovementOffsetYaw : 0.f;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		// FRotator의 Yaw값은 X축을 바라보는 방향으로 0이고 우측으로 180도, 좌측으로 -180도로 구성된다.
		// MovementRotation의 방향에서 AimRotation의 방향을 빼면 AimRotation을 축으로 그 차이만큼 회전한 결과가 된다.
		// 따라서 그 결과인 회전값(-180~180)에 따라 애니메이션이 달라진다.

		bAiming = ShooterCharacter->GetAiming();
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 여기서 ShooterCharacter를 세팅한다. BeginPlay()같은 함수는 모든 객체들이 준비가 된 상태에서 실행되기 때문에
	// 안전성을 확보할 수 있다.
	// TryGetPawnOwner()함수는 현재 애님인스턴스를 소유하고 있는 폰을 반환한다.
	// 다만 반환형이 APawn이라서 캐스팅은 해줘야 한다.
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
