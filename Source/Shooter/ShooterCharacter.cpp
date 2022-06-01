// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (충돌이 있을 경우 캐릭터 쪽으로 끌어당깁니다.)
	// 선언부에서 생성한 컴포넌트 포인터 변수는 구현부에서 CreateDefaultSubobject<>이용해서 실제 컴포넌트를 생성하고 해당 주소를 가리키게 된다.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// ACharacter를 상속한 액터들의 RootComponent는 무조건 CapsuleComponent다.
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;			// The camera follows at the distance behind the character
	// bUsePawnControlRotation을 true로 설정하면 스프링암이 컨트롤러의 회전값을 따라간다.
	CameraBoom->bUsePawnControlRotation = true;		// Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 아래의 함수에서 알 수 있는 것은 비단 카메라뿐만이 아니라 각종 컴포넌트들을 소켓에도 부착할 수 있다는 점.
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 카메라는 스프링암을 기준으로 회전하지 않는다.
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 아래의 세 변수는 캐릭터가 회전할 떄, 같이 따라서 회전하지 않도록 함. 만약 ture면.
	// Dont rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = false;

	// 캐릭터가 이동하는 방향으로 캐릭터의 룩이 회전하게 함. true면.
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate.
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// 어느 방향이 forward인 지 찾는다.
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// 어느 방향이 right인 지 찾는다.
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}

void AShooterCharacter::FireWeapon()
{
	// FireSound는 사운드 큐인데 에디터에서 사운드 에셋을 가지고 큐로 만들어서 사용한다. 
	if (FireSound)
	{
		// 사운드큐를 재생하는 것은 UGameplayStatics에서 구현된 PlaySound2D함수인데 
		// UGameplayStatics클래스는 내부에 게임개발에 필요한 잡다한 것들이 여러가지 있어서 파보면 파볼수록 좋다.
		// 그리고 지금 쓰는 사운드큐의 경우에 10개의 사운드를 랜덤으로 하나 뽑아서 사용하고 있는데 그 과정은 에디터에서 편집했으니
		// 그 과정도 기억하면 좋다.
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	// 스켈레탈 메시에 장착한 소켓을 불러와서 해당 소켓의 위치를 구한 뒤 그 위치에 파티클을 생성한다.
	// 단 여기서는 트랜스폼을 구한 뒤 거기서 Location값을 얻는다.
	// 왜냐면 파티클은 UGameplayStatics::SpawnEmitterAtLocation()에서 생성하는데 이때 파라미터 값으로 트랜스폼을 넘겨야 해서
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(TEXT("BarrelSocket"));
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		// GetWorld()->LineTraceSingleByChannel은 라인트레이싱 함수고 FHitResult를 인자로 넘기면 FHitResult에 계산 결과를 갱신한다
		// 시작과 끝점을 넘기면 그 사이에 뭐랑 부딪쳤는지에 대한 정보를 반환
		// FQuat는 쿼터니온이고 쿼터니온은 회전에 관한 정보를 가지고 있다.
		FHitResult FireHit;
		const FVector Start{ SocketTransform.GetLocation() };
		const FQuat Rotation{ SocketTransform.GetRotation() };
		const FVector RotationAxis{ Rotation.GetAxisX() };
		const FVector End{ Start + RotationAxis * 50'000.f };

		// 얘는 라인트레이싱의 결과와 상관없이 투사체가 나아가는 걸 표현하기 위한 거기 때문에 무조건 그린다
		FVector BeamEndPoint{ End };

		GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (FireHit.bBlockingHit) // bBlockingHit이 true면 라인트레이싱에서 뭔가랑 부딪쳤다는 뜻
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f); // true면 라인이 계속 그려진다는 뜻.false면 그 다음 매개변수만큼 존재하다가 사라짐
			//DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 2.f);

			BeamEndPoint = FireHit.Location;

			// 충돌한 위치에 파티클을 생성한다
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.Location);
			}
		}

		// 빔 렌더링
		if (BeamParticles)
		{
			// UParticleSystemComponent와 UParticleSystem의 차이점을 구분할 것.
			// UParticleSystemComponent는 컴포넌트이고 UParticleSystem은 에셋이다.
			// 에셋을 월드에 놓으려면 먼저 컴포넌트에 해당 에셋을 장착시켜야 한다.
			// UGameplayStatics::SpawnEmitterAtLocation함수도 보면 파티클 에셋을 매개변수로 전달받고 
			// 파티클컴포넌트를 생성한 뒤 거기에 전달받은 에셋을 장착한 후 반환하고 있다.
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
			}
		}
	}

	// 몽타주의 재생은 애님인스턴스로 재생한다.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 입력 컴포넌트가 유효한 지 체크하는 매크로임
	check(PlayerInputComponent);

	//축 매핑은 매 프레임마다 입력이 들어오고 액션 매핑은 유저가 입력을 할 때만 입력이 들어온다
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("FireButton"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FireWeapon);
}

