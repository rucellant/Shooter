// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USoundCue;
class UCurveFloat;
class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class AShooterCharacter;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayNAme = "Damaged"),
	EIR_Common UMETA(DisplayNAme = "Common"),
	EIR_Uncommon UMETA(DisplayNAme = "Uncommon"),
	EIR_Rare UMETA(DisplayNAme = "Rare"),
	EIR_Legendary UMETA(DisplayNAme = "Legendary"),

	EIR_MAX UMETA(DisplayNAme = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayNAme = "Pickup"),
	EIS_EquipInterping UMETA(DisplayNAme = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayNAme = "PickedUp"),
	EIS_Equipped UMETA(DisplayNAme = "Equipped"),
	EIS_Falling UMETA(DisplayNAme = "Falling"),

	EIS_MAX UMETA(DisplayNAme = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayNAme = "Ammo"),
	EIT_Weapon UMETA(DisplayNAme = "Weapon"),

	EIT_MAX UMETA(DisplayNAme = "DefaultMax")
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called when overlapping AreaSphere */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	/** Called when End overlapping AreaSphere */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	/** Sets the ActiveStarts array of bools based on rarity */
	void SetActiveStars();

	/** Sets properties of the Item's components based on State */
	virtual void SetItemProperties(EItemState State);

	/** Called when ItemInterpTimer is finished */
	void FinishInterping();

	/** Handles item interpolation when in the EquipInterping state */
	void ItemInterp(float DeltaTime);

	/** Get interp location based on the item type */
	FVector GetInterpLocation();

	void PlayPickupSound();

	virtual void InitializeCustomDepth();

	// 에디터에서 새로 스폰하거나 이동할 때 호출됨
	virtual void OnConstruction(const FTransform& Transform ) override;

	void EnableGlowMaterial();
	void DisableGlowMaterial();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called in AshooterCharacter::GetPickupItem
	void PlayEquipSound();

private:
	/** Skeletal Mesh for the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** Line trace collides with box to shw HUD widgets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	/** Popup widget for when the player looks at the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	/** Enables item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	/** The name which appears on the Pickup Widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/** Item count (ammo, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/** Item rarity - determins number of stars in Pickup Widget  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** State of the Item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/** The curve asset to use for the imte's Z location when interping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemZCurve;

	/** Starting location when interping begins */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/** Target interp location in front of the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;

	/** true when interping */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;

	/** Plays when we start interping */
	FTimerHandle ItemInterpTimer;

	/** Duration of the curve and timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	/** Pointer tothe character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* Character;

	/** X and Y for the Item while interping in the EquipIntering state */
	float ItemInterpX;
	float ItemInterpY;

	/** Initial Yaw offset betwwen the camera and the interping item */
	float InterpInitialYawOffset;

	/** Curve used to scale the item when interping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	/** Sound Played when Item is picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;

	/** Sound played when the Item is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	/** Enum for the type of item this Item is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	/** Index of the interp location this item is interping to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;

	/** Index for the material we'd like to change at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;

	/** Dynamic instance that we can change at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	/** Material Instance used with the Dyanmic Material instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	/** Calle from the AShooterCharacter class */
	void StartItemCurve(AShooterCharacter* Char);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
};
