// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
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
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 아래의 함수에서 알 수 있는 것은 비단 카메라뿐만이 아니라 각종 컴포넌트들을 소켓에도 부착할 수 있다는 점.
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 카메라는 스프링암을 기준으로 회전하지 않는다.
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

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

}

